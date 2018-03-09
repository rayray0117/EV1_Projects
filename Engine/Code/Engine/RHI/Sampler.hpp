#pragma once
#include "Engine/RHI/RHITypes.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
struct ID3D11SamplerState;
//////////////////////////////////////////////////////////////////////////
class Sampler
{
public:
	Sampler(RHIDevice *owner);
	Sampler(RHIDevice *owner, RHIEnum::FilterMode min_filter, RHIEnum::FilterMode mag_filter);
	~Sampler();

	bool CreateSampler(RHIEnum::FilterMode min_filter, RHIEnum::FilterMode mag_filter);

	RHIDevice* mo_device;
	ID3D11SamplerState* dx_sampler;
private:

};