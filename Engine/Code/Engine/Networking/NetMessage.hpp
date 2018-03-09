#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/BinaryStream.hpp"
#include <array>
//////////////////////////////////////////////////////////////////////////
class NetConnection;
//////////////////////////////////////////////////////////////////////////
constexpr uint MAX_PAYLOAD_SIZE = 1 KiB;
//////////////////////////////////////////////////////////////////////////
class NetMessage : public BinaryStream
{
public:
	NetMessage();
	NetMessage(uint8 type_index);


	virtual bool read_byte(byte_t *out) override;
	virtual bool write_byte(byte_t const &value) override;
	virtual uint read_bytes(void *out_buffer, uint const count) override;
	virtual uint write_bytes(const void* buffer, uint const size) override;
	uint write_string(const std::string& str);
	uint read_string(std::string& str);
public:
	uint8 message_type_index;
	NetConnection *sender;

	std::array<byte_t, MAX_PAYLOAD_SIZE> payload;
	uint payload_bytes_used = 0;
	uint payload_bytes_read = 0;
};
//////////////////////////////////////////////////////////////////////////