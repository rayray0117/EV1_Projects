#pragma once
#include "Engine/RHI/RHITypes.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
struct ID3D11RasterizerState;
//////////////////////////////////////////////////////////////////////////
struct RasterDesc
{
	RHIEnum::CullMode m_cull;
	RHIEnum::FillMode m_fill;
};
//////////////////////////////////////////////////////////////////////////
class RasterState
{
public:
	RasterState(RHIDevice* owner, RHIEnum::CullMode cull = RHIEnum::BACK_CULLING, RHIEnum::FillMode fill = RHIEnum::FILLMODE_SOLID);
	~RasterState();

public:
	RHIDevice* mo_device;
	ID3D11RasterizerState* dx_raster_state;
	RHIEnum::CullMode m_cull;
	RHIEnum::FillMode m_fill;
private:

};

