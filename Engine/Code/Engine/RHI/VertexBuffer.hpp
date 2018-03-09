#pragma once
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/RHI/RHITypes.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
class RHIDeviceContext;
struct ID3D11Buffer;
//////////////////////////////////////////////////////////////////////////
class VertexBuffer
{
public:
	VertexBuffer(RHIDevice* owner, const Vertex3D* vertices, uint vertex_count, RHIEnum::UsageType usage = RHIEnum::USAGE_STATIC_DRAW);
	~VertexBuffer();

	inline bool isValid() const { return (dx_buffer != nullptr); }
	bool setBufferData(RHIDeviceContext* context, const std::vector<Vertex3D>& vertices);
	bool setBufferData(RHIDeviceContext* context, const Vertex3D* vertexArray, int numVertexes);
	

public:
	ID3D11Buffer* dx_buffer;
	uint buffer_size;
	RHIEnum::UsageType usage_type;
private:
	bool create(RHIDevice *device, const Vertex3D* data, uint count);
};