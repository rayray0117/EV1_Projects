#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/RHI/RHITypes.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
class Texture2D;
struct ID3D11DeviceContext;
class Vector4;
class ShaderProgram;
class Sampler;
class VertexBuffer;
class IndexBuffer;
class RasterState;
class ConstantBuffer;
class BlendState;
class DepthStencilState;
class StructuredBuffer;
class Material;
class Shader;
class ComputeJob;
typedef Texture2D UAVTexture;
//////////////////////////////////////////////////////////////////////////
class RHIDeviceContext
{
public:
	RHIDeviceContext(RHIDevice* owner, ID3D11DeviceContext* context);
	~RHIDeviceContext();


	void flush();        // flushes all commands

	// Clears
	void ClearState();  // clears all rendering state
	void ClearRenderTarget(Texture2D* output, const Vector4& color);
	void ClearDepthStencil(Texture2D* output, float depth = 1.0f, unsigned int stencil = 0);

	// Sets
	void SetRenderTarget(Texture2D* renderTarget, Texture2D* depthTarget = nullptr);
	void SetViewport(uint x, uint y, uint width, uint height);
	void SetShaderProgram(ShaderProgram* shader);
	void SetTexture(uint idx, const Texture2D* texture);
	void SetUAVTexture(uint idx, const UAVTexture* texture);
	void SetSampler(uint idx, const Sampler* sampler);
	void SetConstantBuffer(uint idx, const ConstantBuffer *cb);
	void SetRasterState(RasterState* defaultRasterState);
	void SetBlendState(BlendState* blendState);
	void SetDepthStencilState(DepthStencilState* depthStencilState);
	void SetStructuredBuffer(uint bind_point,const StructuredBuffer *buffer);
	void SetMaterial(const Material* mat);
	void SetShader(const Shader* shader);

	// Draws
	void draw(RHIEnum::PrimativeType topology, VertexBuffer* vbo, uint vertex_count, uint start_index = 0);
	void draw_indexed(RHIEnum::PrimativeType topology, VertexBuffer* vbo, IndexBuffer* ibo, uint vertex_count, uint start_index = 0, uint base_vertex = 0);


	// NEW!!
	void Dispatch(ComputeJob* job);

public:
	RHIDevice* mo_device;
	ID3D11DeviceContext* dx_context;
};