#pragma once
#include "Engine/RHI/RHITypes.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
class RHIDeviceContext;
struct ID3D11Buffer;
//////////////////////////////////////////////////////////////////////////
class IndexBuffer
{
public:
	IndexBuffer(RHIDevice *owner, const unsigned int *indices, const unsigned int index_count, RHIEnum::UsageType usage = RHIEnum::USAGE_STATIC_DRAW);
	~IndexBuffer();

	inline bool isValid() const { return (dx_buffer != nullptr); }

	bool setBufferData(RHIDeviceContext* context, const unsigned int* indexArray, int numIndexes);

public:
	ID3D11Buffer* dx_buffer;
private:
	bool create(RHIDevice *device, const unsigned int* indices, const unsigned int index_count);
private:
	RHIEnum::UsageType usage_type;
	unsigned int buffer_size;
};