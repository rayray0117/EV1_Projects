#include "Engine/Networking/TCPConnection.hpp"
#include "Engine/Networking/TCPSocket.hpp"
#include "Engine/Networking/NetMessage.hpp"
//////////////////////////////////////////////////////////////////////////
TCPConnection::TCPConnection()
{

}

TCPConnection::~TCPConnection()
{

}
//////////////////////////////////////////////////////////////////////////
void TCPConnection::Send(NetMessage* msg)
{
	uint16 sendSize = (uint16)(msg->payload_bytes_used + 1);
	m_socket->Send(&sendSize, sizeof(sendSize));
	m_socket->Send(&msg->message_type_index, 1);
	m_socket->Send(msg->payload.data(), msg->payload_bytes_used);
}

void TCPConnection::Receive(NetMessage ** msg)
{
	//m_socket->Receive()
	do 
	{
		if (rtbm < 2)
		{
			int count = m_socket->Receive(buffer.data() + rtbm, 2 - rtbm);
			if (count == 0) return;

			rtbm += count;
		}
		else
		{
			uint msgSize = *(uint16*)&buffer;
			if (rtbm < 2 + msgSize)
			{
				int count = m_socket->Receive(buffer.data() + rtbm, 2 + msgSize - rtbm);

				rtbm += count;
			}
			else
			{
				NetMessage* tempMsg = new NetMessage(buffer[2]);
				std::vector<byte_t> buf;
				buf.resize(msgSize);
				std::memcpy(buf.data(), buffer.data() + 3, msgSize - 1);
				tempMsg->write_bytes(buf.data(), msgSize);
				*msg = tempMsg;
				rtbm = 0;
			}
		}
	} while (rtbm != 0);
}

//////////////////////////////////////////////////////////////////////////
bool TCPConnection::Connect()
{
	return m_socket->Join(address);
}

bool TCPConnection::isDisconnected()
{

	if (m_socket == nullptr || !m_socket->is_valid())
		return true;

	return false;
}

