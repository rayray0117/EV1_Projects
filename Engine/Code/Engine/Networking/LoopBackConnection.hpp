#pragma once
#include "Engine/Networking/NetConnection.hpp"
#include <queue>

//////////////////////////////////////////////////////////////////////////
class LoopBackConnection : public NetConnection
{
public:
	virtual ~LoopBackConnection();

	virtual void Send(NetMessage *msg);		// enqueue
	virtual void Receive(NetMessage **msg);	// dequeue

public:
	std::queue<NetMessage*> m_messages;
};