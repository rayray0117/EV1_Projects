#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Networking/NetConnection.hpp"
#include <array>
//////////////////////////////////////////////////////////////////////////
class TCPSocket;
//////////////////////////////////////////////////////////////////////////
constexpr uint MAX_BUFFER_SIZE = 1 KiB + 3;
//////////////////////////////////////////////////////////////////////////
class TCPConnection : public NetConnection
{
public:
	TCPConnection();
	~TCPConnection();
	
	virtual void Send(NetMessage *msg) override;
	virtual void Receive(NetMessage **msg) override;
	bool Connect();

public:
	TCPSocket* m_socket = nullptr;
	bool isDisconnected();

private:
	std::array<byte_t, MAX_BUFFER_SIZE> buffer;
	uint rtbm = 0;
};