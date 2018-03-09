#pragma once
#include "Engine/RHI/RHITypes.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
struct ID3D11BlendState;
//////////////////////////////////////////////////////////////////////////
class BlendState
{
public:
	BlendState(RHIDevice* owner, RHIEnum::BlendFactor source, RHIEnum::BlendFactor destination, bool enabled = true);
	BlendState(RHIDevice* owner, bool enabled);
	~BlendState();

public:
	RHIDevice* mo_device;
	ID3D11BlendState* dx_state;
};