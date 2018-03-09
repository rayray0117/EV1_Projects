#pragma once
#include "Engine/RHI/RHITypes.hpp"
#include "Engine/Core/EngineBase.hpp"
#include <string>
//////////////////////////////////////////////////////////////////////////
class RHIDeviceContext;    // Potential Display/Background worker
class RHIOutput;
class RHIInstance;  // System level singleton
class Window;
class Texture2D;
class ShaderProgram;
class VertexBuffer;
class IndexBuffer;
struct Vertex3D;
class Sampler;
class ComputeShader;

struct ID3D11Device;
//////////////////////////////////////////////////////////////////////////
// Created from RenderInstance when initial output is created
class RHIDevice
{
public:
	RHIDevice();
	RHIDevice(RHIInstance *owner, ID3D11Device *dxd);
	~RHIDevice();

	inline RHIDeviceContext* getImmediateContext() { return mo_immediate_context; }

 	Texture2D* CreateTextureFromFile(const char* filename);
	Texture2D* CreateTextureFromData(const std::string& name, unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel);
 	// Samplers
 	//Sampler* CreateSampler(eFilterMode min_filter, eFilterMode mag_filter); 
	TODO("See if I actually want to create sampler from RHI");

	ShaderProgram* CreateShaderFromFile_hlsl(char const *filename);
	ShaderProgram* CreateShaderFromString_hlsl(char const *name, const std::string& buffer);

	VertexBuffer* CreateVertexBuffer(const Vertex3D* vertices, uint vertex_count, RHIEnum::UsageType usage = RHIEnum::USAGE_STATIC_DRAW);
	IndexBuffer* CreateIndexBuffer(const uint *indices, uint const index_count, RHIEnum::UsageType usage = RHIEnum::USAGE_STATIC_DRAW);

	// [OPTIONAL] Ability to create secondary outputs
	//            Neat for screensavers, tools, or utlizing a second monitor
 	RHIOutput* CreateOutput(Window* window, RHIEnum::OutputMode mode = RHIEnum::RENDEROUTPUT_WINDOWED);
// 	RHIOutput* create_output(const uint px_width, const uint px_height, eRHIOutputMode const mode);


public:
	// Immediate Mode Context - Created with the Device
	RHIDeviceContext* mo_immediate_context;
	ID3D11Device* dx_device;
	RHIInstance* mo_instance;         // hold a reference so the system doesn't shutdown without me.
};