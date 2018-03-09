#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/DX11.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/RHI/Sampler.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/RHI/RasterState.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/RHI/BlendState.hpp"
#include "DepthStencilState.hpp"
#include "Engine/RHI/StructuredBuffer.hpp"
#include "Engine/RHI/Material.hpp"
#include "Engine/RHI/Shader.hpp"
#include "Engine/RHI/ComputeShader.hpp"
//////////////////////////////////////////////////////////////////////////
RHIDeviceContext::RHIDeviceContext(RHIDevice* owner, ID3D11DeviceContext* context)
	: mo_device(owner)
	, dx_context(context)
{

}

RHIDeviceContext::~RHIDeviceContext()
{
	DX_SAFE_RELEASE(dx_context);
}
//////////////////////////////////////////////////////////////////////////
void RHIDeviceContext::flush()
{
	dx_context->Flush();
}
//////////////////////////////////////////////////////////////////////////
void RHIDeviceContext::ClearState()
{
	dx_context->ClearState();
}

void RHIDeviceContext::ClearRenderTarget(Texture2D* output, const Vector4& color)
{
	dx_context->ClearRenderTargetView(output->dx_render_target, color.getAsFloatArray());
}

void RHIDeviceContext::ClearDepthStencil(Texture2D* output, float depth /*= 1.0f*/, unsigned int stencil /*= 0*/)
{
	if (output != nullptr)
	{
		dx_context->ClearDepthStencilView(output->dx_depth_stencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, (UINT8)stencil);
	}
}

//////////////////////////////////////////////////////////////////////////
void RHIDeviceContext::SetRenderTarget(Texture2D* renderTarget, Texture2D* depthTarget)
{
	if (depthTarget)
	{
		dx_context->OMSetRenderTargets(1, &renderTarget->dx_render_target, depthTarget->dx_depth_stencil);
	}
	else
	{
		dx_context->OMSetRenderTargets(1, &renderTarget->dx_render_target, nullptr);
	}
}

void RHIDeviceContext::SetViewport(uint x, uint y, uint width, uint height)
{
	// Also, set which region of the screen we're rendering to, in this case, all of it 
	D3D11_VIEWPORT viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.TopLeftX = (FLOAT)x;
	viewport.TopLeftY = (FLOAT)y;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;        // must be between 0 and 1 (defualt is 0);
	viewport.MaxDepth = 1.0f;        // must be between 0 and 1 (default is 1)

	dx_context->RSSetViewports(1, &viewport);
}

void RHIDeviceContext::SetShaderProgram(ShaderProgram* shader)
{
	dx_context->VSSetShader(shader->dx_vertex_shader, nullptr, 0U);
	dx_context->PSSetShader(shader->dx_fragment_shader, nullptr, 0U);
	dx_context->IASetInputLayout(shader->dx_input_layout);
}

void RHIDeviceContext::SetTexture(uint idx, const Texture2D* texture)
{
	dx_context->VSSetShaderResources(idx, 1, &texture->dx_shader_resource_view);
	dx_context->PSSetShaderResources(idx, 1, &texture->dx_shader_resource_view);
}

void RHIDeviceContext::SetUAVTexture(uint idx, const UAVTexture* texture)
{
	if (texture == nullptr)
	{
		ID3D11UnorderedAccessView* uav = nullptr;
		dx_context->CSSetUnorderedAccessViews(idx, 1, &uav, nullptr);
	}
	else
	{
		dx_context->CSSetUnorderedAccessViews(idx, 1, &texture->dx_uav, nullptr);
	}
}

void RHIDeviceContext::SetSampler(uint idx, const Sampler* sampler)
{
	dx_context->VSSetSamplers(idx, 1, &sampler->dx_sampler);
	dx_context->PSSetSamplers(idx, 1, &sampler->dx_sampler);
}

void RHIDeviceContext::SetConstantBuffer(uint idx, const ConstantBuffer *cb)
{
	dx_context->VSSetConstantBuffers(idx, 1, &cb->dx_buffer);
	dx_context->PSSetConstantBuffers(idx, 1, &cb->dx_buffer);
}

void RHIDeviceContext::SetRasterState(RasterState* defaultRasterState)
{
	dx_context->RSSetState(defaultRasterState->dx_raster_state);
}

void RHIDeviceContext::SetBlendState(BlendState* blendState)
{
	float constant[] = { 1.f,1.f,1.f,1.f };
	dx_context->OMSetBlendState(blendState->dx_state, constant, 0xffffffff);
}

void RHIDeviceContext::SetDepthStencilState(DepthStencilState* depthStencilState)
{
	dx_context->OMSetDepthStencilState(depthStencilState->dx_state, 0);
}

//////////////////////////////////////////////////////////////////////////
void RHIDeviceContext::draw(RHIEnum::PrimativeType topology, VertexBuffer* vbo, uint vertex_count, uint start_index /*= 0U*/)
{
	D3D11_PRIMITIVE_TOPOLOGY d3d_prim;
	d3d_prim = TypeToD3D(topology);

	dx_context->IASetPrimitiveTopology(d3d_prim);
	uint stride = sizeof(Vertex3D);
	uint offsets = 0;
	dx_context->IASetVertexBuffers(0, 1, &vbo->dx_buffer, &stride, &offsets);

	dx_context->Draw(vertex_count, start_index);
}

void RHIDeviceContext::draw_indexed(RHIEnum::PrimativeType topology, VertexBuffer* vbo, IndexBuffer* ibo, uint index_count, uint start_index /*= 0U*/, uint base_vertex)
{
	D3D11_PRIMITIVE_TOPOLOGY d3d_prim;
	d3d_prim = TypeToD3D(topology);

	dx_context->IASetPrimitiveTopology(d3d_prim);
	uint stride = sizeof(Vertex3D);
	uint offsets = 0;
	dx_context->IASetVertexBuffers(0, 1, &vbo->dx_buffer, &stride, &offsets);
	dx_context->IASetIndexBuffer(ibo->dx_buffer, DXGI_FORMAT_R32_UINT, offsets);

	dx_context->DrawIndexed(index_count, start_index, base_vertex);
}


void RHIDeviceContext::Dispatch(ComputeJob* job)
{
	// at bare minimum - you need to bind the shader
	//TODO("Bind Shader");
	dx_context->CSSetShader(job->shader_to_run->dx_compute_shader, nullptr, 0);
	dx_context->CSSetConstantBuffers(0, 1, &job->const_buffer_main->dx_buffer);
	// depending on optionals, you would also bind any properties/resources needed [inputs and outputs]
	// For the bare minimum of the assignment, you can have these set before calling dispatch.  For the advanced
	// version - the state is stored with the compute job.
	//OPTIONAL_TODO("Bind Job State");

	// Then call dispatch
	dx_context->Dispatch(job->grid_size.x, job->grid_size.y, job->grid_size.z);

	// Since resources can not be bound as both inputs and outputs at the same time - I usually
	// have output (read/write) resources unbind after the job dispatch has been called.
	// Can be part of dispatch if dispatch also handles binding it.  Otherwise you
	// will unbind outside of this function before you need to use the resource... for example
	// when clearing, the texture being cleared is bound as a target.  To render that texture we 
	// then need to unbind it from the output of the compute pipepline, and bind it as the
	// input (ie, set_texture(...)) of a render pipeline (or other compute job...)
	//OPTIONAL_TOD("Unbind Job State")
}

void RHIDeviceContext::SetStructuredBuffer(uint bind_point,const StructuredBuffer *buffer)
{
	// todo: check for null, and unbind if so.
	dx_context->VSSetShaderResources(bind_point, 1, &buffer->dx_srv);
	dx_context->PSSetShaderResources(bind_point, 1, &buffer->dx_srv);
}

void RHIDeviceContext::SetMaterial(const Material* mat)
{
	if (nullptr == mat) {
		return;
	}

	SetShader(mat->mo_shader);
	for (uint i = 0; i < mat->mo_textures.size(); ++i) 
	{
		if(mat->mo_textures[i]) //#ToDo: figure out which I'd usually mean when I send a null texture. Am I wanting it to be null or just not changed?
			SetTexture(i, mat->mo_textures[i]);
	}
	/*/
	for (uint i = 0; i < mat->property_blocks.get_count(); ++i) {
		PropertyBlock *block = mat->property_blocks[i];
		if (nullptr != block) {
			block->update(this);
			set_constant_buffer(block->get_index(), block);
		}
	}
	//*/
}

void RHIDeviceContext::SetShader(const Shader* shader)
{
	if (shader == nullptr) {
		SetShaderProgram(nullptr);
		SetRasterState(nullptr);
		SetDepthStencilState(nullptr);
		SetBlendState(nullptr);
	}
	else {
		SetShaderProgram(shader->mo_shaderProgram);
		SetRasterState(shader->m_rasterState);
		SetDepthStencilState(shader->m_depthStencilState);
		SetBlendState(shader->m_blendState);
	}

}
