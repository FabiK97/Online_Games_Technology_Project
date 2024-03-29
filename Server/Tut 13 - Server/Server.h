#pragma once
#include <WinSock2.h> //For win sockets
#include <string> //For std::string
#include "FileTransferData.h" //for FileTransferData class
#include "PacketManager.h" //for PacketManager class
#include <vector> //for std::vector
#include <shared_mutex> //for shared_mutex
#include "Dot.h"

class Connection
{
public:
	Connection(SOCKET socket) 
		:m_socket(socket)
	{
	}
	SOCKET m_socket;
	//file transfer data
	FileTransferData m_file; //Object that contains information about our file that is being sent to the client from this server
	PacketManager m_pm; //Packet Manager for outgoing data for this connection
	int m_ID = 0;
	bool auth = false;
	void setAuth() {
		auth = true;
	};
	bool isAuth() {
		return auth;
	};
};

class Server
{
public: //Public functions
	Server(int port, bool loopBacktoLocalHost = true);
	~Server();
	bool ListenForNewConnection();
private: //Private functions
	bool sendall(std::shared_ptr<Connection> connection, const char * data, const int totalBytes);
	bool recvall(std::shared_ptr<Connection> connection, char * data, int totalBytes);
	bool Getint32_t(std::shared_ptr<Connection> connection, std::int32_t & int32_t);
	bool GetPacketType(std::shared_ptr<Connection> connection, PacketType & packetType);
	void SendString(std::shared_ptr<Connection> connection, const std::string & str);
	void SendAuth(std::shared_ptr<Connection> connection);
	void SendDot(std::shared_ptr<Connection> connection, Dot & dot);
	void SendDotInit(std::shared_ptr<Connection> connection);
	bool GetString(std::shared_ptr<Connection> connection, std::string & str);
	bool ProcessPacket(std::shared_ptr<Connection> connection, PacketType packetType);
	std::string addId(int id, std::string message);
	static void ClientHandlerThread(Server & server, std::shared_ptr<Connection> connection);
	static void PacketSenderThread(Server & server); 
	void DisconnectClient(std::shared_ptr<Connection> connection); //Called to properly disconnect and clean up a client (if possible)
private: //Private Variables
	std::vector<std::shared_ptr<Connection>> m_connections;
	std::shared_mutex m_mutex_connectionMgr; //mutex for managing connections (used when a client disconnects)
	int m_IDCounter = 0;
	SOCKADDR_IN m_addr; //Address that we will bind our listening socket to
	SOCKET m_sListen;
	bool m_terminateThreads = false;
	std::vector<std::thread*> m_threads; //so destructor can wait on created threads to end...
};
