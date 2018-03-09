#include "Engine/RHI/Shader.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/RHI/BlendState.hpp"
#include "Engine/RHI/DepthStencilState.hpp"
#include "Engine/RHI/RasterState.hpp"
#include "Engine/RHI/RHIDevice.hpp"
//////////////////////////////////////////////////////////////////////////
struct blend_state
{
	bool enabled;
	RHIEnum::BlendFactor src_factor;
	RHIEnum::BlendFactor dest_factor;
};
//////////////////////////////////////////////////////////////////////////
Shader::Shader(RHIDevice* owner)
	: mo_device(owner)
	, m_rasterState(nullptr)
	, m_blendState(nullptr)
	, m_depthStencilState(nullptr)
	, mo_shaderProgram(nullptr)
{
}

Shader::~Shader()
{
	SAFE_DELETE(m_rasterState);
	SAFE_DELETE(m_blendState);
	SAFE_DELETE(m_depthStencilState);
}
//////////////////////////////////////////////////////////////////////////

bool Shader::create_from_program(ShaderProgram *program, RasterDesc const *opt_raster_state /*= nullptr*/, DepthStencilDesc const *opt_ds_state /*= nullptr*/, blend_state const *opt_blend_state /*= nullptr*/)
{
	if (!program)
		return false;

	mo_shaderProgram = program;

	if (opt_blend_state)
		m_blendState = new BlendState(mo_device, opt_blend_state->src_factor, opt_blend_state->dest_factor, opt_blend_state->enabled);

	if (opt_ds_state)
		m_depthStencilState = new DepthStencilState(mo_device, *opt_ds_state);

	if (opt_raster_state)
		m_rasterState = new RasterState(mo_device, opt_raster_state->m_cull, opt_raster_state->m_fill);

	return true;
}

bool Shader::create_from_program_file(char const *filename, RasterDesc const *opt_raster_state /*= nullptr*/, DepthStencilDesc const *opt_ds_state /*= nullptr*/, blend_state const *opt_blend_state /*= nullptr*/)
{
	mo_shaderProgram = mo_device->CreateShaderFromFile_hlsl(filename);

	if (opt_blend_state)
		m_blendState = new BlendState(mo_device, opt_blend_state->src_factor, opt_blend_state->dest_factor, opt_blend_state->enabled);

	if (opt_ds_state)
		m_depthStencilState = new DepthStencilState(mo_device, *opt_ds_state);

	if (opt_raster_state)
		m_rasterState = new RasterState(mo_device, opt_raster_state->m_cull, opt_raster_state->m_fill);

	return true;
}
