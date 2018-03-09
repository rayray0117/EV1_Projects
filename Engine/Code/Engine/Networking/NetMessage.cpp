#include "Engine/Networking/NetMessage.hpp"
#include "Engine/Networking/NetConnection.hpp"
#include <array>
//////////////////////////////////////////////////////////////////////////
NetMessage::NetMessage()
	: message_type_index(NET_MSG_INVALID)
{
	stream_order = LITTLE_ENDIAN;
}

NetMessage::NetMessage(uint8 type_index)
	: message_type_index(type_index)
{
}
//////////////////////////////////////////////////////////////////////////
bool NetMessage::read_byte(byte_t* out)
{
	return (read_bytes(out, 1) == 1);
}

bool NetMessage::write_byte(byte_t const &value)
{
	return (write_bytes(&value, 1) == 1);
}

uint NetMessage::write_bytes(const void* buffer, uint const size)
{
	uint new_size = size;
	if (new_size + payload_bytes_used > MAX_PAYLOAD_SIZE)
	{
		DebuggerPrintf("NetMessage::write_bytes buffer too big truncating!");
		new_size = MAX_PAYLOAD_SIZE;
	}

	std::memcpy(payload.data() + payload_bytes_used, buffer, new_size);
	payload_bytes_used += new_size;
	return new_size;
}

uint NetMessage::read_bytes(void *out_buffer, uint const count)
{
	uint new_size = count;
	if (new_size + payload_bytes_read > MAX_PAYLOAD_SIZE)
	{
		DebuggerPrintf("NetMessage::read_bytes buffer too big truncating!");
		new_size = MAX_PAYLOAD_SIZE;
	}


	std::memcpy(out_buffer, payload.data()+ payload_bytes_read, new_size);
	payload_bytes_read += new_size;
	return new_size;
}

uint NetMessage::write_string(const std::string& str)
{
	uint16 msgSize = uint16(str.length());
	if (msgSize > 0xfffe)
		msgSize = 0xfffe;

	*(uint16*)payload.data() = msgSize;
	
	payload_bytes_used = msgSize + 2;
	std::memcpy(payload.data() + 2, str.data(), msgSize);
	return (uint)msgSize;
}

uint NetMessage::read_string(std::string& str)
{
	uint16 msgSize = *(uint16*)payload.data();
	for (uint16 i = 0; i < msgSize; ++i)
	{
		str += payload[2 + i];
	}
	return (uint)msgSize+2;
}
//////////////////////////////////////////////////////////////////////////