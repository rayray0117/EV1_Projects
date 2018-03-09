#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

IndexBuffer::IndexBuffer(RHIDevice *owner, const unsigned int *indices, const unsigned int index_count, RHIEnum::UsageType usage /*= D3DEnum::USAGE_STATIC_DRAW*/)
	:usage_type(usage)
{
	dx_buffer = nullptr;
	buffer_size = 0;

	create(owner, indices, index_count);
}

IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE(dx_buffer);
}

bool IndexBuffer::setBufferData(RHIDeviceContext* context, const unsigned int* indexArray, int numIndexes)
{
	// Make sure we are dynamic before trying to change things
	if (usage_type != RHIEnum::USAGE_DYNAMIC_DRAW)
	{
		ERROR_RECOVERABLE("WARNING: Tried to set buffer data to a static vbo");
		return false;
	}


	//Check if we have enough space for update and increase size if not.
	uint size = sizeof(uint) *numIndexes;
	if (size > buffer_size)
	{
		return create(context->mo_device, indexArray, numIndexes);
	}
	// Else do regular update
	D3D11_MAPPED_SUBRESOURCE resource;
	ID3D11DeviceContext *dx_context = context->dx_context;
	if (SUCCEEDED(dx_context->Map(dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) {
		memcpy(resource.pData, indexArray, sizeof(unsigned int) *numIndexes);
		dx_context->Unmap(dx_buffer, 0);

		return true;
	}

	// ASSERT?
	return false;
}

bool IndexBuffer::create(RHIDevice *device, const unsigned int* indices, const unsigned int index_count)
{
	DX_SAFE_RELEASE(dx_buffer);

	buffer_size = index_count * sizeof(uint);


	D3D11_BUFFER_DESC ib_desc;
	memset(&ib_desc, 0, sizeof(ib_desc));

	ib_desc.ByteWidth = buffer_size;
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;    // different from above - now it is used as an Index Buffer
	
	ib_desc.Usage = TypeToD3D(usage_type);
	if (usage_type == RHIEnum::USAGE_DYNAMIC_DRAW)
		ib_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ib_desc.StructureByteStride = sizeof(uint);

	D3D11_SUBRESOURCE_DATA initial_data;
	D3D11_SUBRESOURCE_DATA *initial_data_ptr = nullptr;

	if (indices != nullptr)
	{
		memset(&initial_data, 0, sizeof(initial_data));
		initial_data.pSysMem = indices;
		initial_data_ptr = &initial_data;
	}

	// Finally create the vertex buffer
	dx_buffer = nullptr;
	device->dx_device->CreateBuffer(&ib_desc, initial_data_ptr, &dx_buffer);

	if (dx_buffer == nullptr)
	{
		buffer_size = 0;
		return false;
	}
	else
	{
		return true;
	}
}
