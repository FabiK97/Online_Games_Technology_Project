#include "PacketStructs.h"

namespace PS
{
	ChatMessage::ChatMessage(const std::string & msg)
		:m_message(msg)
	{
	}

	std::shared_ptr<Packet> ChatMessage::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(PacketType::ChatMessage);
		p->Append(m_message.size());
		p->Append(m_message);
		return p;
	}

	std::shared_ptr<Packet> FileDataBuffer::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(PacketType::FileTransferByteBuffer);
		p->Append(m_size);
		p->Append(m_databuffer, m_size);
		return p;
	}

	DotMessage::DotMessage(Dot d) :dot(d)
	{
	}

	DotMessage::DotMessage(std::string& str) :dotString(str)
	{
	}

	std::shared_ptr<Packet> DotMessage::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		if (dotString.size() <= 0) {
			dotString = dot.GetDotAsString();
		}
		p->Append(PacketType::DotMessage);
		p->Append(dotString.size());
		p->Append(dotString);
		return p;
	}
	AuthoritiveMessage::AuthoritiveMessage(bool b) :auth(b)
	{
	}
	std::shared_ptr<Packet> AuthoritiveMessage::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(PacketType::AuthoritiveMessage);
		return p;
	}
	ChaserMessage::ChaserMessage(std::string& m) :chaserString(m)
	{
	}
	std::shared_ptr<Packet> ChaserMessage::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(chaserString.size());
		p->Append(chaserString);
		return p;
	}
}