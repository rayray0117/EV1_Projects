#include "Engine/Networking/TCPSocket.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
TCPSocket* TCPSocket::Accept()
{
	if (!isListening())
		return nullptr;

	sockaddr_storage conn_addr;
	int conn_addr_len = sizeof(conn_addr);

	SOCKET their_socket = ::accept(m_socket, (sockaddr*)&conn_addr, &conn_addr_len);
	if (their_socket == INVALID_SOCKET)
	{
		return nullptr;
	}

	net_address net_addr;
	if (!NetAddressFromSocketAddress(&net_addr, (sockaddr*)&conn_addr))
	{
		::closesocket(their_socket);
		return nullptr;
	}

	TCPSocket* thier_tcp_socket = new TCPSocket();
	thier_tcp_socket->m_socket = their_socket;
	thier_tcp_socket->m_address = net_addr;
	return thier_tcp_socket;
}

uint  TCPSocket::Send(const void* payload, int size)
{
	if (!is_valid())
		return 0;

	if (size == 0)
		return 0;

	if (isListening())
	{
		ERROR_AND_DIE("Trying to Send on a ListenSocket");
	}

	int bytes_sent = ::send(m_socket, (const char*)payload, (int)size, 0);
	if (bytes_sent <= 0)
	{
		int error = WSAGetLastError();
		DebuggerPrintf("TCPSocket::send failed Error %u\n", error);
		close();
		return 0;
	}

	ASSERT_OR_DIE(bytes_sent == size, "ERROR: bytes_sent != size");
	return bytes_sent;

}

uint TCPSocket::Receive(void* payload, int maxSize)
{
	if (!is_valid() || maxSize == 0)
		return 0;

	if (isListening())
	{
		ERROR_AND_DIE("Trying to Receive on a ListenSocket");
	}

	ASSERT_OR_DIE(payload != nullptr, "");

	int bytes_read = ::recv(m_socket, (char*)payload, (int)maxSize, 0);
	if (bytes_read <= 0)
	{
		if (bytes_read == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				return 0;
			}
			else
			{
				close();
			}
		}
		else
		{
			CheckForDisconnect();
			return 0;
		}
		// for now, with blocking sockets - this is how we detect closure
		return 0;
	}
	else
	{
		return (uint)bytes_read;
	}
}

bool TCPSocket::Listen(uint16 port)
{
	if (is_valid())
	{
		return false;
	}

	std::vector<net_address> addresses = GetAddressFromHostname("", port, true);
	if (addresses.empty())
	{
		return false;
	}

	SOCKET listen_socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_socket == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_storage listen_address;
	uint addr_size = 0;
	SocketAddressFromNetAddress((sockaddr*)&listen_address, &addr_size, addresses[0]);


	// Associate address to this socket;
	int result = ::bind(listen_socket, (sockaddr*)&listen_address, (int)addr_size);
	if (result == SOCKET_ERROR)
	{
		::closesocket(listen_socket);
		return false;
	}

	int max_queued = 8; //Max amount of connections that are waiting to be connected. Anymore than is waiting will just be disconnected right away
	result = ::listen(listen_socket, (int)max_queued);
	if (result == SOCKET_ERROR)
	{
		::closesocket(listen_socket);
		return false;
	}


	m_socket = listen_socket;
	m_address = addresses[0];
	m_isListenSocket = true;
	return true;
}

bool TCPSocket::Join(net_address addr)
{
	if (is_valid())
	{
		return false;
	}

	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_storage  internal_address;
	uint addr_size = 0;
	SocketAddressFromNetAddress((sockaddr*)&internal_address, &addr_size, addr);

	int result = ::connect(sock, (sockaddr*)&internal_address, sizeof(sockaddr_in));
	if (result != 0)
	{
		int error = WSAGetLastError();
		g_theConsole->addMessage(Stringf("ERROR # %i", error));
		::closesocket(sock);
		return false;
	}
	else
	{
		m_socket = sock;
		m_address = addr;
		return true;
	}
}

void TCPSocket::SetBlocking(bool blocking)
{
	if (!is_valid())
		return;

	u_long non_blocking = blocking ? 0 : 1;
	::ioctlsocket(m_socket, FIONBIO, &non_blocking);
}

void  TCPSocket::CheckForDisconnect()
{
	if (!is_valid())
	{
		return;
	}

	WSAPOLLFD fd;
	fd.fd = m_socket;
	fd.events = POLLRDNORM;

	if (SOCKET_ERROR == WSAPoll(&fd, 1, 0))
	{
		return;
	}

	if ((fd.revents & POLLHUP) != 0)
	{
		close();
	}
}
