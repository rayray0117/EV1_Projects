#pragma once
#include "Engine/Networking/Net.hpp"
//////////////////////////////////////////////////////////////////////////
class NetSession;
class NetMessage;
//////////////////////////////////////////////////////////////////////////
class NetConnection
{
public:
	virtual ~NetConnection() {}

	virtual void Send(NetMessage *msg) = 0;
	virtual void Receive(NetMessage **msg) = 0;

public:
	NetSession* owner = nullptr;

	net_address address;
	uint8 connection_index; // LUID 
};