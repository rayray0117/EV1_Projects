#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

bool NetSystemStartup();
void NetSystemShutdown();

#include <vector>

struct net_address
{
	//host name
	// port
	uint address;

	uint16 port; //uint16

	net_address() {}
	net_address(uint addr, uint16 p) : address(addr), port(p) {}
};
const net_address INVALID_ADDRESS(0, 0);
const uint16 INVALID_NETWORK_ID = 0;

void SocketAddressFromNetAddress(sockaddr* out, uint* out_size, const net_address& addr);
std::vector<net_address> GetAddressFromHostname(const char* hostname, uint16 port, bool bindable);
net_address GetMyAddress(uint16 port);
bool NetAddressFromSocketAddress(net_address* out, sockaddr* socket_address);

#include <string>
std::string NetAddressToString(net_address addr);
net_address StringToNetAddress(const std::string& str);

constexpr uint8 INVALID_CONNECTION_INDEX = 0xff;
constexpr uint16 RCS_PORT = 0xabbd;


enum ECoreNetMessage : uint8
{
	JOIN_RESPONSE = 0,

	RCS_COMMAND = 32,
	RCS_ECHO = 33,
	NET_MSG_INVALID = 34,
	NUM_CORE_MESSAGES
};

namespace Net
{
	typedef uint16 portId;
	typedef uint8 luid;
}