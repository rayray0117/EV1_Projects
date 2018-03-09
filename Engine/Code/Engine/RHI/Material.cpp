#include "Engine/RHI/Material.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
//////////////////////////////////////////////////////////////////////////
constexpr int MAX_TEXTURE_BINDS = 10;
constexpr int MAX_CONSTBUFFER_BINDS = 10;
//////////////////////////////////////////////////////////////////////////
Material::Material(RHIDevice* owner)
	: mo_device(owner)
	, mo_shader(nullptr)
{
	mo_textures.reserve(MAX_TEXTURE_BINDS);
	for (int i = 0; i < MAX_TEXTURE_BINDS; i++)
	{
		mo_textures.push_back(nullptr);
	}

	mo_buffers.reserve(MAX_CONSTBUFFER_BINDS);
	for (int i = 0; i < MAX_CONSTBUFFER_BINDS; i++)
	{
		mo_buffers.push_back(nullptr);
	}
}

Material::~Material()
{
}
//////////////////////////////////////////////////////////////////////////
void Material::SetShader(Shader* shader)
{
	mo_shader = shader;
}

void Material::SetTexture(uint bindIndex, Texture2D* resource)
{
	mo_textures[bindIndex] = resource;
}

void Material::SetConstantBuffer(uint bindIndex, ConstantBuffer* buffer)
{
	mo_buffers[bindIndex] = buffer;
}
