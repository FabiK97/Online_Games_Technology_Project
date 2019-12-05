#include "Client.h"
#include <Ws2tcpip.h> //for inet_pton
#pragma comment(lib,"ws2_32.lib") //Required for WinSock
#include <iostream> //for std::cout


using namespace std;

bool Client::ProcessPacketType(PacketType packetType)
{
	switch (packetType)
	{
	case PacketType::ChatMessage: //If PacketType is a chat message PacketType
	{
		std::string Message; //string to store our message we received
		if (!GetString(Message)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false
		std::cout << Message << std::endl; //Display the message to the user
		break;
	}
	case PacketType::DotMessage:
	{
		Dot dot;
		std::string dotm; //string to store our message we received
		if (!GetString(dotm)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false
		dot = processMessage(dotm);

		if (dot.GetCenterX() == NULL) {
			std::cout << "x is null \n";
		} else if (!playerDot->isInit) {
			/*playerDot->setID(dot.getID());
			playerDot->SetPosition(dot.GetCenterX(), dot.GetCenterY());
			playerDot->isChaser = dot.isChaser;

			playerDot->isInit = true;*/
			playerDot->setID(dot.getID());
			playerDot->SetPosition(dot.GetCenterX(), dot.GetCenterY());
			playerDot->isChaser = dot.isChaser;
			playerDot->isInit = true;
		} else if (!enemy1Dot->isInit) {
			enemy1Dot->setID(dot.getID());
			enemy1Dot->SetPosition(dot.GetCenterX(), dot.GetCenterY());
			enemy1Dot->isChaser = dot.isChaser;
			enemy1Dot->isInit = true;
		} else {
			if (enemy1Dot->getID() == dot.getID()) {
				enemy1Dot->SetPosition(dot.GetCenterX(), dot.GetCenterY());
			} else if (!enemy2Dot->isInit) {
				enemy2Dot->setID(dot.getID());
				enemy2Dot->SetPosition(dot.GetCenterX(), dot.GetCenterY());
				enemy2Dot->isChaser = dot.isChaser;
				enemy2Dot->isInit = true;
			} else {
				if (enemy2Dot->getID() == dot.getID()) {
					enemy2Dot->SetPosition(dot.GetCenterX(), dot.GetCenterY());
				}
			}
		}

		break;
	}
	case PacketType::AuthoritiveMessage: 
	{
		isAuthoritive = true;
	}
	case PacketType::ChaserMessage:
	{
		std::string chaserm; //string to store our message we received
		if (!GetString(chaserm)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false
		
		int id;
		stringstream  ss(chaserm);
		string str;
		getline(ss, str, ':');
		string key = str;
		getline(ss, str, ':');
		string value = str;

		if (key == "id") {
			std::stringstream ss(value);
			ss >> id;

			if (playerDot->getID() == id) {
				playerDot->isChaser = true;
				enemy1Dot->isChaser = false;
				enemy2Dot->isChaser = false;
			} else if (enemy1Dot->getID() == id) {
				playerDot->isChaser = false;
				enemy1Dot->isChaser = true;
				enemy2Dot->isChaser = false;
			} else if (enemy2Dot->getID() == id) {
				playerDot->isChaser = false;
				enemy1Dot->isChaser = false;
				enemy2Dot->isChaser = true;
			}

		}
		else {
			return false;
		}

		

	}
	case PacketType::FileTransferByteBuffer:
	{
		std::int32_t buffersize; //buffer to hold size of buffer to write to file
		if (!Getint32_t(buffersize)) //get size of buffer as integer
			return false;
		if (buffersize > FileTransferData::m_bufferSize) //If invalid buffer size (too large)
			return false;
		if (!recvall(m_file.m_buffer, buffersize)) //get buffer and store it in file.buffer
			return false;
		m_file.m_outfileStream.write(m_file.m_buffer, buffersize); //write buffer from file.buffer to our outfilestream
		m_file.m_bytesWritten += buffersize; //increment byteswritten
		std::cout << "Received byte buffer for file transfer of size: " << buffersize << std::endl;
		m_pm.Append(std::make_shared<Packet>(Packet(PacketType::FileTransferRequestNextBuffer)));
		break;
	}
	case PacketType::FileTransfer_EndOfFile:
	{
		std::cout << "File transfer completed. File received.\n";
		std::cout << "File Name: " << m_file.m_fileName << "\n";
		std::cout << "File Size(bytes): " << m_file.m_bytesWritten << std::endl;
		m_file.m_transferInProgress = false;
		m_file.m_bytesWritten = 0;
		m_file.m_outfileStream.close(); //close file after we are done writing file
		break;
	}
	default: //If PacketType type is not accounted for
		std::cout << "Unrecognized PacketType: " << (std::int32_t)packetType << std::endl; //Display that PacketType was not found
		break;
	}
	return true;
}

Dot Client::processMessage(std::string message)
{
	int xCoord = 0;
	int yCoord = 0;
	int id;
	bool chaser = false;

	vector<string> results;

	stringstream  ss(message);
	string str;
	while (getline(ss, str, ',')) {
		results.push_back(str);
	}

	for (auto word : results) {
		stringstream  pair_ss(word);
		string pair_str;
		getline(pair_ss, pair_str, ':');
		string key = pair_str;
		getline(pair_ss, pair_str, ':');
		string value = pair_str;

		if (key == "x") {
			std::stringstream ss(value);
			ss >> xCoord;
		}
		else if (key == "y") {
			std::stringstream ss(value);
			ss >> yCoord;
		}
		else if (key == "id") {
			std::stringstream ss(value);
			ss >> id;
		}
		else if (key == "chaser") {
			int temp;
			std::stringstream ss(value);
			ss >> temp;
			chaser = (temp == 1);
		}
	}


	//create new dot
	Dot dot(chaser);
	dot.setID(id);
	dot.SetPosition(xCoord, yCoord);
	

	return dot;
}

void Client::ClientThread(Client & client)
{
	PacketType PacketType;
	while (true)
	{
		if (client.m_terminateThreads == true)
			break;
		if (!client.GetPacketType(PacketType)) //Get PacketType type
			break; //If there is an issue getting the PacketType type, exit this loop
		if (!client.ProcessPacketType(PacketType)) //Process PacketType (PacketType type)
			break; //If there is an issue processing the PacketType, exit this loop
	}
	std::cout << "Lost connection to the server.\n";
	client.m_terminateThreads = true;
	if (client.CloseConnection()) //Try to close socket connection..., If connection socket was closed properly
	{
		std::cout << "Socket to the server was closed successfully." << std::endl;
	}
	else //If connection socket was not closed properly for some reason from our function
	{
		std::cout << "Socket was not able to be closed." << std::endl;
	}
}

bool Client::RequestFile(const std::string & fileName)
{
	if (m_file.m_transferInProgress)
	{
		std::cout << "ERROR: Function(Client::RequestFile) - File Transfer already in progress.\n";
		return false;
	}
	m_file.m_transferInProgress = true;
	m_file.m_outfileStream.open(fileName, std::ios::binary); //open file to write file to
	m_file.m_fileName = fileName; //save file name
	m_file.m_bytesWritten = 0; //reset byteswritten to 0 since we are working with a new file
	if (!m_file.m_outfileStream.is_open()) //if file failed to open...
	{
		std::cout << "ERROR: Function(Client::RequestFile) - Unable to open file: " << fileName << " for writing.\n";
		return false;
	}
	std::cout << "Requesting file from server: " << fileName << std::endl;
	Packet requestFilePacket;
	requestFilePacket.Append(PacketType::FileTransferRequestFile);
	requestFilePacket.Append(m_file.m_fileName.size());
	requestFilePacket.Append(m_file.m_fileName.c_str(), m_file.m_fileName.size());
	m_pm.Append(std::make_shared<Packet>(requestFilePacket));
	return true;
}

void Client::PacketSenderThread(Client & client) //Thread for all outgoing packets
{
	while (true)
	{
		if (client.m_terminateThreads == true)
			break;
		while (client.m_pm.HasPendingPackets())
		{
			std::shared_ptr<Packet> p = client.m_pm.Retrieve();
			if (!client.sendall((const char*)(&p->m_buffer[0]), p->m_buffer.size()))
			{
				std::cout << "Failed to send packet to server..." << std::endl;
				break;
			}
		}
		Sleep(5);
	}
	std::cout << "Packet thread closing..." << std::endl;
}

void Client::Disconnect()
{
	m_pm.Clear();
	closesocket(m_connection);
	std::cout << "Disconnected from server." << std::endl;
}