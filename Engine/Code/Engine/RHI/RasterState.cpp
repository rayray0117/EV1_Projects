#include "Engine/RHI/RasterState.hpp"
#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
//////////////////////////////////////////////////////////////////////////
RasterState::RasterState(RHIDevice* owner, RHIEnum::CullMode cull, RHIEnum::FillMode fill)
	: mo_device(owner)
	, dx_raster_state(nullptr)
	, m_cull(cull)
	, m_fill(fill)
{
	D3D11_RASTERIZER_DESC desc;
	MemZero(&desc);

	desc.FillMode = TypeToD3D(m_fill);
	desc.CullMode = TypeToD3D(m_cull);

	desc.FrontCounterClockwise = TRUE; //Counter clockwise like OpenGL

	desc.AntialiasedLineEnable = FALSE; //Enable for smooth lines, but costs more for it.
	desc.DepthBias = 0; 
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE; //Clips out anything not it the place
	desc.MultisampleEnable = FALSE;

	HRESULT result = mo_device->dx_device->CreateRasterizerState(&desc, &dx_raster_state);
	UNUSED(result);
	//ASSERT_OR_DIE(result, "ERROR: RasterState Failed");
}

RasterState::~RasterState()
{
	DX_SAFE_RELEASE(dx_raster_state);
}