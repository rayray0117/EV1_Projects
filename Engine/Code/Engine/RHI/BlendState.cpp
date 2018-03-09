#include "Engine/RHI/BlendState.hpp"	
#include "Engine/RHI/DX11.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/RHI/RHIDevice.hpp"
//////////////////////////////////////////////////////////////////////////
BlendState::BlendState(RHIDevice* owner, RHIEnum::BlendFactor source, RHIEnum::BlendFactor destination, bool enabled)
	: mo_device(owner)
	, dx_state(nullptr)
{
	D3D11_BLEND_DESC desc;
	MemZero(&desc);

	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;

	desc.RenderTarget[0].BlendEnable = enabled;

	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlend = TypeToD3D(source);
	desc.RenderTarget[0].DestBlend = TypeToD3D(destination);

	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11Device* dx_device = owner->dx_device;
	HRESULT hr = dx_device->CreateBlendState(&desc, &dx_state);

	if (FAILED(hr))
	{
		//Assert OR die
		dx_state = nullptr;
	}
}

BlendState::BlendState(RHIDevice* owner, bool enabled)
{
	D3D11_BLEND_DESC desc;
	MemZero(&desc);

	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;

	desc.RenderTarget[0].BlendEnable = enabled;

	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlend = TypeToD3D(RHIEnum::BLEND_ONE);
	desc.RenderTarget[0].DestBlend = TypeToD3D(RHIEnum::BLEND_ZERO);

	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11Device* dx_device = owner->dx_device;
	HRESULT hr = dx_device->CreateBlendState(&desc, &dx_state);

	if (FAILED(hr))
	{
		//Assert OR die
		dx_state = nullptr;
	}
}

BlendState::~BlendState()
{
	DX_SAFE_RELEASE(dx_state);
}
