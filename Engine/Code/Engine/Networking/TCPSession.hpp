#pragma once
#include "Engine/Networking/NetSession.hpp"
#include "Engine/Networking/Net.hpp"
//////////////////////////////////////////////////////////////////////////
class TCPSocket;
class NetConnection;
class NetMessage;
//////////////////////////////////////////////////////////////////////////
class TCPSession : public NetSession
{
public:
	TCPSession() {}
	virtual ~TCPSession();
	
public:
	// create a loopback - add it to connections, set my and host connection to it;
	virtual bool Host(uint16 port) override;

	virtual bool Join(net_address const &addr) override; // production version: session_join_info_t instead

														   // if I have a connection - destroy it.
	virtual void Leave() override;

	// Process all connections; 
	virtual void Update() override;


	void SendJoinInfo(NetConnection* cp);
	void OnJoinResponse(NetMessage* msg);

	bool StartListening() override;
	void StopListening() override;
	bool isListening() const override
	{
		return (nullptr != listen_socket);
	}
public:
	TCPSocket* listen_socket = nullptr;
};