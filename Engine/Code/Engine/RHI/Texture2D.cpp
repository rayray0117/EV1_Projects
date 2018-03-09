#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/DX11.hpp"
#include "Engine/Core/Image.hpp"
//////////////////////////////////////////////////////////////////////////
Texture2D::Texture2D(RHIDevice* owner)
	: mo_device(owner)
	, dx_texture(nullptr)
	, dx_render_target(nullptr)
	, dx_shader_resource_view(nullptr)
	, dx_depth_stencil(nullptr)
	, m_width(0)
	, m_height(0)
	, dx_bind_flags(0U)
	, m_imageFilePath("UNKNOWN IMAGE FILE")
{}

Texture2D::Texture2D(RHIDevice* owner, ID3D11Texture2D* tex)
	: mo_device(owner)
	, dx_texture(tex)
	, dx_render_target(nullptr)
	, dx_shader_resource_view(nullptr)
	, dx_depth_stencil(nullptr)
	, m_width(0)
	, m_height(0)
	, dx_bind_flags(0U)
	, m_imageFilePath("UNKNOWN IMAGE FILE")
{
	if(tex != nullptr) {
		tex->AddRef();
		dx_texture = tex;

		D3D11_TEXTURE2D_DESC desc;
		tex->GetDesc(&desc);

		m_width = desc.Width;
		m_height = desc.Height;
		dx_bind_flags = desc.BindFlags;

		create_views();
	}
}

Texture2D::Texture2D(RHIDevice* owner, const char* filename)
	: mo_device(owner)
	, dx_texture(nullptr)
	, dx_render_target(nullptr)
	, dx_shader_resource_view(nullptr)
	, dx_depth_stencil(nullptr)
	, m_width(0)
	, m_height(0)
	, dx_bind_flags(0U)
	, m_imageFilePath(filename)
{
	Image image(filename);
	setFromImage(image);
}

Texture2D::Texture2D(RHIDevice* owner, uint width, uint height, RHIEnum::ImageFormat format, bool isUAV)
	:mo_device(owner)
	, dx_texture(nullptr)
	, dx_render_target(nullptr)
	, dx_shader_resource_view(nullptr)
	, dx_depth_stencil(nullptr)
	, m_width(width)
	, m_height(height)
	, dx_bind_flags(0U)
	, m_imageFilePath("UNKNOWN IMAGE FILE")
{
	// default usage - this is going to be written to by the GPU
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	width = m_width;
	height = m_height;

	DXGI_FORMAT dx_format;
	uint dx_bindings = 0U;
	switch (format) {
	case RHIEnum::IMAGEFORMAT_RGBA8:
		dx_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dx_bindings = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		if(isUAV)
			dx_bindings |= D3D11_BIND_UNORDERED_ACCESS;
		break;
	case RHIEnum::IMAGEFORMAT_D24S8:
		// depth textures are 24-bit depth, 8-bit stencil
		dx_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dx_bindings = D3D11_BIND_DEPTH_STENCIL; // binding this to as a shader resource requires a couple extra steps - saved for later
		break;
	default:
		// ASSERTORDIE
		return;
	};

	D3D11_TEXTURE2D_DESC tex_desc;
	memset(&tex_desc, 0, sizeof(tex_desc));

	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.MipLevels = 1;                // no mip mapping
	tex_desc.ArraySize = 1;                // NO ARRAYS!  
	tex_desc.Usage = usage;
	tex_desc.Format = dx_format;
	tex_desc.BindFlags = dx_bindings;
	tex_desc.CPUAccessFlags = 0U;
	if (isUAV)
		tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	tex_desc.MiscFlags = 0U; // there is one for generating mip maps.

							 // multisampling options
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;

	// no initial data - we're creating renderable targets

	ID3D11Device *dx_device = owner->dx_device;
	HRESULT hr = dx_device->CreateTexture2D(&tex_desc, nullptr, &dx_texture);

	if (SUCCEEDED(hr)) {
		dx_bind_flags = dx_bindings;
		create_views();
	}
}

Texture2D::~Texture2D()
{
	Destroy();
}

unsigned int Texture2D::getWidth() const
{ 
	return m_width;
}

unsigned int Texture2D::getHeight() const
{
	return m_height;
}

bool Texture2D::setFromImage(const Image& img)
{
	D3D11_USAGE usage = UsageTypeToD3D(RHIEnum::USAGE_STATIC_DRAW);

	D3D11_TEXTURE2D_DESC tex_desc;
	memset(&tex_desc, 0, sizeof(tex_desc));

	tex_desc.Width = img.m_dimensions.x;
	tex_desc.Height = img.m_dimensions.y;
	tex_desc.MipLevels = 1; //No mip mapping
	tex_desc.ArraySize = 1; // NO ARRAYS
	tex_desc.Usage = usage;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0U;
	tex_desc.MiscFlags = 0U;

	// multisampling
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;

	// Helps us figure out what views to make
	dx_bind_flags = D3D11_BIND_SHADER_RESOURCE;
	m_width = img.m_dimensions.x;
	m_height = img.m_dimensions.y;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));

	data.pSysMem = img.m_texelBytes;
	data.SysMemPitch = img.m_bytesPerTexel * tex_desc.Width;

	ID3D11Device *dx_device = mo_device->dx_device;
	HRESULT hr = dx_device->CreateTexture2D(&tex_desc, &data, &dx_texture);

	if (SUCCEEDED(hr)) {
		create_views();
		return true;
	}
	else {
		return false;
	}
}

bool Texture2D::setFromData(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel)
{
	D3D11_USAGE usage = UsageTypeToD3D(RHIEnum::USAGE_STATIC_DRAW);

	D3D11_TEXTURE2D_DESC tex_desc;
	memset(&tex_desc, 0, sizeof(tex_desc));

	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.MipLevels = 1; //No mip mapping
	tex_desc.ArraySize = 1; // NO ARRAYS
	tex_desc.Usage = usage;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0U;
	tex_desc.MiscFlags = 0U;

	// multisampling
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;

	// Helps us figure out what views to make
	dx_bind_flags = D3D11_BIND_SHADER_RESOURCE;
	m_width = width;
	m_height = height;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));

	data.pSysMem = imageTexelBytes;
	data.SysMemPitch = bytesPerTexel * tex_desc.Width;

	ID3D11Device *dx_device = mo_device->dx_device;
	HRESULT hr = dx_device->CreateTexture2D(&tex_desc, &data, &dx_texture);

	if (SUCCEEDED(hr)) {
		create_views();
		return true;
	}
	else {
		return false;
	}
}

void Texture2D::Destroy()
{
	DX_SAFE_RELEASE(dx_shader_resource_view);
	DX_SAFE_RELEASE(dx_render_target);
	DX_SAFE_RELEASE(dx_texture);
	DX_SAFE_RELEASE(dx_depth_stencil);
}

void Texture2D::create_views()
{
	ID3D11Device *dd = mo_device->dx_device;

	if (dx_bind_flags & D3D11_BIND_RENDER_TARGET) 
	{
		dd->CreateRenderTargetView(dx_texture, nullptr, &dx_render_target);
	}

	if (dx_bind_flags & D3D11_BIND_SHADER_RESOURCE) 
	{
		dd->CreateShaderResourceView(dx_texture, nullptr, &dx_shader_resource_view);
	}

	if (dx_bind_flags & D3D11_BIND_DEPTH_STENCIL) 
	{
		dd->CreateDepthStencilView(dx_texture, nullptr, &dx_depth_stencil);
	}

	if (dx_bind_flags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		MemZero(&desc);

		desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;;
		desc.Texture2D.MipSlice = 0;

		// NOTE:  Depth Stencil Buffers need to have a different view
		// then their normal 
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dd->CreateUnorderedAccessView(dx_texture, &desc, &dx_uav);
	}
}