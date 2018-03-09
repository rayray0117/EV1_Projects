#include "Engine/Networking/TCPSession.hpp"
#include "Engine/Networking/LoopBackConnection.hpp"
#include "Engine/Networking/TCPConnection.hpp"
#include "Engine/Networking/TCPSocket.hpp"
#include "Engine/Networking/NetMessage.hpp"
#include "NetMessageDefinition.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
TCPSession::~TCPSession()
{

}
//////////////////////////////////////////////////////////////////////////
bool TCPSession::Host(uint16 port)
{
	ASSERT_OR_DIE(!isRunning(), "");

	my_connection = new LoopBackConnection();
	my_connection->address = GetMyAddress(port);

	JoinConnection(0, my_connection);
	host_connection = my_connection;

	SetState(SESSION_CONNECTED);

	Console::GetInstance()->addMessage(Stringf("Hosting on %s", NetAddressToString(GetMyAddress(port)).c_str()));

	return true;
}

bool TCPSession::Join(net_address const &addr)
{
	TCPConnection* host = new TCPConnection();
	host->address = addr;
	host->owner = this;

	JoinConnection(getFreeConnectionIndex(), host); // 0 for this class;
	host->m_socket = new TCPSocket();
	host_connection = host;

	// Try to connect to host
	if (!host->Connect()) 
	{
		Leave();
		return false;
	}

	host->m_socket->SetBlocking(false);
	my_connection = new LoopBackConnection();
	my_connection->owner = this;
	my_connection->address = GetMyAddress(addr.port); //can also be invalid

	SetState(SESSION_CONNECTING);
	return true;
}

void TCPSession::Leave()
{
	DestroyConnection(my_connection);
	DestroyConnection(host_connection);

	for (uint i = 0; i < m_connections.size(); ++i)
	{
		if(m_connections[i] == nullptr)
			continue;

		DestroyConnection(m_connections[i]);
	}

	StopListening();
	SetState(SESSION_DISCONNECTED);
}

void TCPSession::Update()
{
	if (isListening()) 
	{
		TCPSocket *socket = listen_socket->Accept();
		if (socket != nullptr) 
		{
			listen_socket->SetBlocking(false);
			TCPConnection *new_guy = new TCPConnection();
			new_guy->m_socket = socket;
			new_guy->address = socket->m_address;
			new_guy->owner = this;

			uint8_t conn_idx = getFreeConnectionIndex();
			if (conn_idx == INVALID_CONNECTION_INDEX)
			{
				delete new_guy;
			}
			else 
			{
				new_guy->connection_index = conn_idx;
				new_guy->owner->SetState(ESessionState::SESSION_CONNECTED);
				JoinConnection(conn_idx, new_guy);
				SendJoinInfo(new_guy);
			}
		}
	}

	// You already have this;
	// foreach (connection) {
	//   receive messages 
	// } 
	for (uint i = 0; i < m_connections.size(); ++i)
	{
		NetMessage* msg = nullptr;

		if(nullptr == m_connections[i])
			continue;

		m_connections[i]->Receive(&msg);
		while (nullptr != msg)
		{
			msg->sender = m_connections[i];
			NetMessageDefinition* def = get_message_definition(msg->message_type_index);
			if (nullptr != def)
			{
				def->handler(msg);
			}
			SAFE_DELETE(msg);
			m_connections[i]->Receive(&msg);
		}
	}

	for (uint i = 0; i < m_connections.size(); ++i) 
	{
		NetConnection *cp = m_connections[i];
		if ((cp != nullptr) && (cp != my_connection)) 
		{
			TCPConnection *tcp_connection = (TCPConnection*)cp;
			// TODO: is_disconnected excercise to reader
			if (tcp_connection->isDisconnected()) 
			{
				DestroyConnection(tcp_connection);
				tcp_connection = nullptr;
			}
		}
	}

	if (host_connection == nullptr) 
	{
		Leave();
	}
}

void TCPSession::SendJoinInfo(NetConnection* cp)
{
	NetMessage* msg = new NetMessage(JOIN_RESPONSE);
	msg->write<uint8>(cp->connection_index);

	cp->Send(msg);
}

void TCPSession::OnJoinResponse(NetMessage* msg)
{
	uint8 my_conn_index;
	msg->read<uint8>(my_conn_index);

	JoinConnection(my_conn_index, my_connection);
	SetState(SESSION_CONNECTED);
}

bool TCPSession::StartListening()
{
	if (!isHost()) {
		return false;
	}

	if (isListening()) {
		return true;
	}

	listen_socket = new TCPSocket();
	if (listen_socket->Listen(my_connection->address.port)) 
	{
		listen_socket->SetBlocking(false);
		return true;
	}
	else {
		delete listen_socket;
		listen_socket = nullptr;
		return false;
	}
}

void TCPSession::StopListening()
{
	if (isListening()) 
	{
		delete listen_socket;
		listen_socket = nullptr;
	}
}

