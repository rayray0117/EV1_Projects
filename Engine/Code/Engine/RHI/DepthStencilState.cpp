#include "Engine/RHI/DepthStencilState.hpp"
#include "Engine/RHI/DX11.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/RHI/RHIDevice.hpp"

DepthStencilState::DepthStencilState(RHIDevice* owner, DepthStencilDesc desc)
	: mo_device(owner)
	, dx_state(nullptr)
{
	D3D11_DEPTH_STENCIL_DESC dxdesc;
	MemZero(&dxdesc);

	dxdesc.DepthEnable = desc.depthTestEnabled || desc.depthWritingEnabled;
	dxdesc.DepthWriteMask = desc.depthWritingEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dxdesc.DepthFunc = TypeToD3D(desc.depthFunction);
	dxdesc.StencilEnable = FALSE;

	HRESULT result = mo_device->dx_device->CreateDepthStencilState(&dxdesc, &dx_state);
	UNUSED(result);
}

DepthStencilState::~DepthStencilState()
{
	DX_SAFE_RELEASE(dx_state);
}
