#include "Engine/Networking/NetSession.hpp"
#include "Engine/Networking/NetConnection.hpp"
#include "Engine/Networking/NetMessage.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "NetMessageDefinition.hpp"
//////////////////////////////////////////////////////////////////////////
bool NetSession::register_message_function(uint8 msg_id, std::function<void(NetMessage*)> /*Event<void(NetMessage*)>*/ const &callback)
{
	if (get_message_definition(msg_id) != nullptr)
		return false;

	NetMessageDefinition* msg = new NetMessageDefinition();
	msg->handler = callback;
	msg->type_index = msg_id;
	message_defintions.push_back(msg);
	return true;
}

NetMessageDefinition* NetSession::get_message_definition(uint8 id) const
{
	for (uint i = 0; i < message_defintions.size(); ++i)
	{
		if(message_defintions[i]->type_index == id)
			return message_defintions[i];
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
uint8 NetSession::getFreeConnectionIndex() const
{
	uint index = 0;
	for (index = 0; index < m_connections.size(); ++index) 
	{
		if (m_connections[index] == nullptr) 
		{
			return (uint8)index;
		}
	}

	if (index < m_maxConnectionCount) 
	{
		return (uint8)index;
	}
	else 
	{
		return INVALID_CONNECTION_INDEX;
	}
}

void NetSession::JoinConnection(uint8 idx, NetConnection* conn)
{
	conn->connection_index = idx;

	ASSERT_OR_DIE((idx >= m_connections.size()) || (m_connections[idx] == nullptr),"Trying to join null connection");

	if (idx >= m_connections.size()) 
	{
		m_connections.resize(idx + 1);
		m_connections[idx] = nullptr;
	}

	m_connections[idx] = conn;
	m_connectionList.add(conn);
}

void NetSession::DestroyConnection(NetConnection* cp)
{
	if (nullptr == cp)
		return;

	if (my_connection == cp)
		my_connection = nullptr;


	if (host_connection == cp)
		host_connection = nullptr;


	if (cp->connection_index != INVALID_CONNECTION_INDEX) 
	{
		m_connections[cp->connection_index] = nullptr;
		cp->connection_index = INVALID_CONNECTION_INDEX;
	}

	m_connectionList.remove(cp);

	delete cp;
}

NetConnection* NetSession::getConnection(uint8 index)
{
	if (index < m_connections.size())
		return m_connections[index];

	return nullptr;
}

void NetSession::SendMessageToIndex(uint8 index, const NetMessage& msg)
{
	NetConnection* cp = getConnection(index);
	
	if (nullptr != cp)
		cp->Send(new NetMessage(msg));
}

void NetSession::SendMessageToOthers(const NetMessage& msg)
{
	LinkNode<NetConnection*>* cp = m_connectionList.m_head;
	for (uint i = 0; i < m_connectionList.length; ++i)
	{
		if (cp->data != my_connection)
		{
			cp->data->Send(new NetMessage(msg));
		}
		cp = cp->sibling_right;
	}
}

void NetSession::SendMessageToAll(const NetMessage& msg)
{
	LinkNode<NetConnection*>* cp = m_connectionList.m_head;
	for (uint i = 0; i < m_connectionList.length; ++i)
	{
		cp->data->Send(new NetMessage(msg));
		cp = cp->sibling_right;
	}
}

