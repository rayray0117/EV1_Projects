#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
class RHIDeviceContext;

struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
// DEFINITION
//------------------------------------------------------------------------
class StructuredBuffer
{
public:
	// Note, it takes an object size, and object_count instead
	// of the arryas total size, as object_size matters in 
	// creation!
	StructuredBuffer(RHIDevice *owner, void const *buffer, uint object_size, uint object_count);
	~StructuredBuffer();

	// New for a structured buffer!
	bool create_views();

	bool update(RHIDeviceContext *context, void const *buffer);

	inline bool is_valid() const { return (dx_buffer != nullptr); }

public:
	RHIDevice* mo_device;

	ID3D11Buffer* dx_buffer;
	ID3D11ShaderResourceView* dx_srv;

	uint obj_count;
	uint obj_size;
	size_t buffer_size;
};

