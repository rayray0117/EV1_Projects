#pragma once
#include "Engine/RHI/RHITypes.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
struct ID3D11DepthStencilState;
//////////////////////////////////////////////////////////////////////////
struct DepthStencilDesc
{
	bool depthWritingEnabled;
	bool depthTestEnabled;
	RHIEnum::DepthFunction depthFunction;
};
//////////////////////////////////////////////////////////////////////////
class DepthStencilState
{
public:
	DepthStencilState(RHIDevice* owner, DepthStencilDesc desc);
	~DepthStencilState();

public:
	RHIDevice* mo_device;
	ID3D11DepthStencilState* dx_state;
};