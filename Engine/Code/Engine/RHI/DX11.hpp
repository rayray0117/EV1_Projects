#pragma once
//------------------------------------------------------------------------
// DX11.*
// Used for including most D3D11 related files/libs,
// as well as supplying conversion functions.
//------------------------------------------------------------------------
#include "Engine/RHI/RHITypes.hpp"

#include <d3d11.h>
//#include <d3d10.h>
#include <DXGI.h>

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

// LIBS
#pragma comment( lib, "d3d11.lib" )
//#pragma comment( lib, "d3d10.lib" )
#pragma comment( lib, "DXGI.lib" )

// nothing else for now

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

// NEEDED FOR COMPILING
// Note:  This is not allowed for Windows Store Apps.
// Shaders must be compiled to byte-code offline. 
// but for development - great if the program knows how to compile it.
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )


D3D11_PRIMITIVE_TOPOLOGY PrimativeTypeToD3D(RHIEnum::PrimativeType type);
D3D11_PRIMITIVE_TOPOLOGY TypeToD3D(RHIEnum::PrimativeType type);
bool PrimativeTypeValid(RHIEnum::PrimativeType type);
bool TypeValid(RHIEnum::PrimativeType type);

D3D11_USAGE UsageTypeToD3D(RHIEnum::UsageType type);
D3D11_USAGE TypeToD3D(RHIEnum::UsageType type);

D3D11_FILTER FilterModeToD3D(RHIEnum::FilterMode min_filter, RHIEnum::FilterMode mag_filter);
D3D11_FILTER TypeToD3D(RHIEnum::FilterMode min_filter, RHIEnum::FilterMode mag_filter);

D3D11_BLEND TypeToD3D(RHIEnum::BlendFactor factor);

D3D11_CULL_MODE CullModeToD3D(RHIEnum::CullMode cull);
D3D11_CULL_MODE TypeToD3D(RHIEnum::CullMode cull);

D3D11_FILL_MODE FillModeToD3D(RHIEnum::FillMode fill);
D3D11_FILL_MODE TypeToD3D(RHIEnum::FillMode fill);

D3D11_COMPARISON_FUNC DepthFuncToD3D(RHIEnum::DepthFunction depthFunc);
D3D11_COMPARISON_FUNC TypeToD3D(RHIEnum::DepthFunction depthFunc);