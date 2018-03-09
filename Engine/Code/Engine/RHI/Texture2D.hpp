#pragma once
#include <string>
#include "Engine/RHI/RHITypes.hpp"
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct ID3D11UnorderedAccessView;
class Image;
//////////////////////////////////////////////////////////////////////////
class Texture2D
{
	friend class RHIDevice;
	friend class RHIOutput;
	friend class SimpleRenderer;
public:
	~Texture2D();

	unsigned int getWidth() const;
	unsigned int getHeight() const;

	inline bool isValid() const { return (dx_texture != nullptr); }
	inline bool isRenderTarget() const { return (dx_render_target != nullptr); }
	
	void Destroy();

public:
	RHIDevice* mo_device;

	ID3D11Texture2D* dx_texture;
	ID3D11RenderTargetView* dx_render_target;
	ID3D11ShaderResourceView* dx_shader_resource_view;
	ID3D11DepthStencilView* dx_depth_stencil;
	ID3D11UnorderedAccessView* dx_uav;
	

	unsigned int dx_bind_flags;
private:
	unsigned int m_width;
	unsigned int m_height;
	std::string m_imageFilePath;

private:
	Texture2D(RHIDevice* owner);
	Texture2D(RHIDevice* owner, ID3D11Texture2D* tex);
	Texture2D(RHIDevice* owner, const char* filename);
	Texture2D(RHIDevice* owner, uint width, uint height, RHIEnum::ImageFormat format, bool isUAV = false);

	bool setFromImage(const Image& img);
	bool setFromData(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel);
	void create_views();
};
//////////////////////////////////////////////////////////////////////////
typedef Texture2D UAVTexture;