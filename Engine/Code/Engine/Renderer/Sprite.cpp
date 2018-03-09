#include "Engine/Renderer/Sprite.hpp"
#include "Engine/Core/XMLParseHelper.h"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
//////////////////////////////////////////////////////////////////////////
const std::string NO_TEXTURE_SET = "";
//////////////////////////////////////////////////////////////////////////
Sprite::Sprite(const XMLNode& element)
	: DataDrivenDefinition(element)
{
	ParseXMLAttribute(element, "uv", m_uv);
	std::string texturePath = ParseXMLAttribute(element, "texture", NO_TEXTURE_SET);
	ASSERT_OR_DIE(texturePath != NO_TEXTURE_SET, Stringf("No texture set for %s", m_name.c_str()));
	m_spriteTexture = g_mainEngineRenderer->CreateOrGetTexture(texturePath);
}

Sprite::Sprite(const std::string& name)
	: DataDrivenDefinition(name)
{

}

const Texture2D& Sprite::getTexture() const
{

	return *m_spriteTexture;
}
