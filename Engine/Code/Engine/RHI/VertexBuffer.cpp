#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/RHI/DX11.hpp"


VertexBuffer::VertexBuffer(RHIDevice* owner, const Vertex3D* vertices, uint vertex_count, RHIEnum::UsageType usage)
	:usage_type(usage)
{
	dx_buffer = nullptr;
	buffer_size = 0;

	create(owner, vertices, vertex_count);
}

//------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	DX_SAFE_RELEASE(dx_buffer);
}

bool VertexBuffer::setBufferData(RHIDeviceContext* context, const std::vector<Vertex3D>& vertices)
{
	return setBufferData(context, vertices.data(), vertices.size());
}

bool VertexBuffer::setBufferData(RHIDeviceContext* context, const Vertex3D* vertexArray, int numVertexes)
{
	// Make sure we are dynamic before trying to change things
	if (usage_type != RHIEnum::USAGE_DYNAMIC_DRAW)
	{
		//ERROR_RECOVERABLE("WARNING: Tried to set buffer data to a static vbo");
		return false;
	}

	//Check if we have enough space for update and increase size if not.
	uint size = sizeof(Vertex3D) *numVertexes;
	if (size > buffer_size)
	{
		return create(context->mo_device, vertexArray, numVertexes);
	}
	// Else do regular update
	D3D11_MAPPED_SUBRESOURCE resource;
	ID3D11DeviceContext *dx_context = context->dx_context;
	if (SUCCEEDED(dx_context->Map(dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource)))
	{
		memcpy(resource.pData, vertexArray, size);
		dx_context->Unmap(dx_buffer, 0);

		return true;
	}

	return false;
}

bool VertexBuffer::create(RHIDevice *device, const Vertex3D* data, uint count)
{
	DX_SAFE_RELEASE(dx_buffer);

	buffer_size = count * sizeof(Vertex3D);

	// First, describe the buffer
	D3D11_BUFFER_DESC vb_desc;
	memset(&vb_desc, 0, sizeof(vb_desc));

	vb_desc.ByteWidth = buffer_size;  // How much data are we putting into this buffer
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)

	vb_desc.Usage = TypeToD3D(usage_type);
	if (usage_type == RHIEnum::USAGE_DYNAMIC_DRAW)
		vb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	vb_desc.StructureByteStride = sizeof(Vertex3D);       // How large is each element in this buffer

														  // Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initial_data;
	D3D11_SUBRESOURCE_DATA *initial_data_ptr = nullptr;

	if (data != nullptr)
	{
		memset(&initial_data, 0, sizeof(initial_data));
		initial_data.pSysMem = data;
		initial_data_ptr = &initial_data;
	}

	// Finally create the vertex buffer
	
	device->dx_device->CreateBuffer(&vb_desc, initial_data_ptr, &dx_buffer);

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
