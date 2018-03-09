#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
struct ID3D11Buffer;
class RHIDeviceContext;
//////////////////////////////////////////////////////////////////////////
class ConstantBuffer
{
public:
	ConstantBuffer(RHIDevice* owner, const void* buffer, const uint size);
	~ConstantBuffer();

	inline bool isValid() const { return (dx_buffer != nullptr); }

	bool update(RHIDeviceContext* owner, const void* buffer);

public:
	ID3D11Buffer *dx_buffer;
	uint buffer_size;
};