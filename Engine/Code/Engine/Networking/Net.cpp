#include "Engine/Networking/Net.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
net_address StringToNetAddress(const std::string& str)
{
	std::vector<std::string> ip_and_port;
	splitByChar(str, ip_and_port, ':');

	std::string ip = ip_and_port[0];
	uint16 port = (uint16)stoul(ip_and_port[1]);

	std::vector<std::string> ip_nums;
	splitByChar(ip, ip_nums, '.');

	net_address ad;


	uchar* ips = (uchar*)&ad.address;
	ips[0] = (uchar)stoul(ip_nums[0]);
	ips[1] = (uchar)stoul(ip_nums[1]);
	ips[2] = (uchar)stoul(ip_nums[2]);
	ips[3] = (uchar)stoul(ip_nums[3]);

	ad.port = port;
	ad.address = (ips[0]<<24) | (ips[1]<<16) | (ips[2]<<8) | ips[3];

	return ad;
}

void SocketAddressFromNetAddress(sockaddr* out, uint* out_size, const net_address& addr)
{
	sockaddr_in *out_addr = (sockaddr_in*)out;
	out_addr->sin_family = AF_INET;
	out_addr->sin_port = htons((u_short)addr.port);
	out_addr->sin_addr.S_un.S_addr = htonl(addr.address);

	*out_size = sizeof(sockaddr_in); //Only supports IPv4
}

std::vector<net_address> GetAddressFromHostname(const char* hostname, uint16 port, bool bindable)
{
	char service[16];
	sprintf_s(service, "%u", port);

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; //Set to IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP = SOCK_STREAM, UDP = SOCK_DGRAM

	hints.ai_flags |= (bindable ? AI_PASSIVE : 0);
	//hints.ai_flags |= AI_NUMERICHOST; //If only using regular numbered IP Addresses


	addrinfo *results = nullptr;
	int status = ::getaddrinfo(hostname, service, &hints, &results);

	std::vector<net_address> addresses;
	if (status != 0)
	{
		//ERROR happened
		return addresses;
	}

	addrinfo* addr = results;
	while (nullptr != addr)
	{
		net_address net_addr;
		if (NetAddressFromSocketAddress(&net_addr, addr->ai_addr))
		{
			addresses.push_back(net_addr);
		}

		addr = addr->ai_next;
	}

	::freeaddrinfo(results);
	return addresses;
}

net_address GetMyAddress(uint16 port)
{
	std::vector<net_address> addresses = GetAddressFromHostname("", port, true);
	if (addresses.empty())
		return INVALID_ADDRESS;
	
	return addresses[0];
}

bool NetAddressFromSocketAddress(net_address* out, sockaddr* socket_address)
{
	if (nullptr == socket_address)
		return false;

	if (socket_address->sa_family != AF_INET) //Currently only support IPv4
	{
		return false;
	}

	sockaddr_in* ipv4_address = (sockaddr_in*)socket_address;

	out->port = ntohs(ipv4_address->sin_port); //ntohs - Net to Host Short
	out->address = ntohl(ipv4_address->sin_addr.S_un.S_addr);

	return true;
}

std::string NetAddressToString(net_address addr)
{
	uchar* ar = (uchar*)&addr.address;

	uchar a = ar[0];
	uchar b = ar[1];
	uchar c = ar[2];
	uchar d = ar[3];

	std::string str = std::to_string(d) + "." + std::to_string(c) + "." + std::to_string(b) + "." + std::to_string(a) + ":" + std::to_string(addr.port);

	return str;
}

bool NetSystemStartup()
{
	WORD version = MAKEWORD(2, 2);

	WSADATA data;
	int error = ::WSAStartup(version, &data);
	return (error == 0);
}

void NetSystemShutdown()
{
	::WSACleanup();
}