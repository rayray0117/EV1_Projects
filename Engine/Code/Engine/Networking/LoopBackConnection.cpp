#include "Engine/Networking/LoopBackConnection.hpp"
//////////////////////////////////////////////////////////////////////////
LoopBackConnection::~LoopBackConnection()
{
}
//////////////////////////////////////////////////////////////////////////
void LoopBackConnection::Send(NetMessage *msg)
{
	m_messages.push(msg);
}

void LoopBackConnection::Receive(NetMessage **msg)
{
	if (m_messages.empty())
		return;

	msg = &m_messages.back();
	m_messages.pop();
}

