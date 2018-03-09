#include "Engine/RHI/Sampler.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/DX11.hpp"

Sampler::Sampler(RHIDevice *owner)
	: mo_device(owner)
	, dx_sampler(nullptr)
{
}

Sampler::Sampler(RHIDevice *owner, RHIEnum::FilterMode min_filter, RHIEnum::FilterMode mag_filter)
	: mo_device(owner)
	, dx_sampler(nullptr)
{
	CreateSampler(min_filter, mag_filter);
}

Sampler::~Sampler()
{
	DX_SAFE_RELEASE(dx_sampler);
}

bool Sampler::CreateSampler(RHIEnum::FilterMode min_filter, RHIEnum::FilterMode mag_filter)
{
	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.Filter = FilterModeToD3D(min_filter, mag_filter);
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //Will repeat when it goes off. Can make this changable.
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//desc.BorderColor = BLACK TRANSPARENT IF 0;
	//Set both to 0 for just the first level
	desc.MinLOD = 0;
	desc.MaxLOD = 0;

	ID3D11Device *dx_device = mo_device->dx_device;
	HRESULT hr = dx_device->CreateSamplerState(&desc, &dx_sampler);

	return SUCCEEDED(hr);
}
