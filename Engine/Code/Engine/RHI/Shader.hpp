#pragma once
#include <string>
#include "Engine/RHI/BlendState.hpp"
#include "Engine/RHI/RasterState.hpp"
#include "Engine/RHI/DepthStencilState.hpp"
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
class ShaderProgram;
struct blend_state;
//////////////////////////////////////////////////////////////////////////
class Shader
{
public:
	Shader(RHIDevice* owner);
	~Shader();
	
	//*/
	// bool load_from_file( char const *filename );
	// if state is set to nullptr, it creates one 
	// with default settings.
	bool create_from_program(ShaderProgram *program,
		RasterDesc const *opt_raster_state = nullptr,
		DepthStencilDesc const *opt_ds_state = nullptr,
		blend_state const *opt_blend_state = nullptr);

	// tries to create a program from an *.hlsl file
	bool create_from_program_file(char const *filename,
		RasterDesc const *opt_raster_state = nullptr,
		DepthStencilDesc const *opt_ds_state = nullptr,
		blend_state const *opt_blend_state = nullptr);
	//*/
public:
	RHIDevice* mo_device;

	ShaderProgram* mo_shaderProgram;
	RasterState* m_rasterState;
	BlendState* m_blendState;
	DepthStencilState* m_depthStencilState;
};