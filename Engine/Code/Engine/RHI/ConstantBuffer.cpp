#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"


ConstantBuffer::ConstantBuffer(RHIDevice* owner, const void* buffer, const uint size)
	:buffer_size(size)
{
	// First, describe the buffer
	D3D11_BUFFER_DESC vb_desc;
	memset(&vb_desc, 0, sizeof(vb_desc));

	vb_desc.ByteWidth = size;  // How much data are we putting into this buffer
	vb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;         // What can we bind this data as (in this case, only vertex data)
	vb_desc.Usage = D3D11_USAGE_DYNAMIC;                // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
															  // for limitations/strenghts of each, see;  
															  //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	vb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	vb_desc.StructureByteStride = 0;       // const buffers don't need stride

														  // Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initial_data;
	memset(&initial_data, 0, sizeof(initial_data));
	initial_data.pSysMem = buffer;

	// Finally create the vertex buffer
	dx_buffer = nullptr;
	owner->dx_device->CreateBuffer(&vb_desc, &initial_data, &dx_buffer);
}

//------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{
	DX_SAFE_RELEASE(dx_buffer);
}


bool ConstantBuffer::update(RHIDeviceContext* context, const void* buffer)
{
	D3D11_MAPPED_SUBRESOURCE resource;

	ID3D11DeviceContext *dx_context = context->dx_context;
	if (SUCCEEDED(dx_context->Map(dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) {
		memcpy(resource.pData, buffer, buffer_size);
		dx_context->Unmap(dx_buffer, 0);

		return true;
	}

	// ASSERT?
	return false;
}

