#pragma once
#include "Engine/Networking/Net.hpp"


//////////////////////////////////////////////////////////////////////////
class TCPSocket
{
public:
	TCPSocket() : m_socket(INVALID_SOCKET) {}
	~TCPSocket() { close(); }

	//Client
	bool Join(net_address addr);
	bool is_valid() const
	{
		return m_socket != INVALID_SOCKET;
	}

	void close()
	{
		if (is_valid())
		{
			::closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			m_isListenSocket = false;
		}
	}

	// Host
	bool Host(uint port);
	TCPSocket* Accept();

	//Both
	uint Send(const void* payload, int size);
	uint Receive(void* payload, int maxSize);

	void SetBlocking(bool blocking);
	void CheckForDisconnect();


	bool Listen(uint16 port);

	bool isListening() const
	{
		return m_isListenSocket;
	}
public:
	SOCKET m_socket = INVALID_SOCKET;
	bool m_isListenSocket = false;
	net_address m_address;
private:
};
