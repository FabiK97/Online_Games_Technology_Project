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

	std::shared_ptr<Packet> DotMessage::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		std::string dotString = dot.GetDotAsString();
		p->Append(PacketType::DotMessage);
		p->Append(dotString.size());
		p->Append(dotString);
		return p;
	}
	AuthoritiveMessage::AuthoritiveMessage(const std::string& str) :m_message(str)
	{
	}
	std::shared_ptr<Packet> AuthoritiveMessage::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(PacketType::DotMessage);
		p->Append(m_message.size());
		p->Append(m_message);
		return p;
	}
}