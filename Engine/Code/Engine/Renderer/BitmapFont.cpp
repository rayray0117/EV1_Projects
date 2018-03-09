#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/EngineBase.hpp"


BitmapFont::BitmapFont(const std::string& bitmapFontName, const Texture& texture)
	: m_spriteSheet(new SpriteSheet(texture, 16, 16))
	, m_name(bitmapFontName)
	, m_textureFilePath("Data/Fonts/" + bitmapFontName + ".png")
{
}

BitmapFont::BitmapFont(const std::string& bitmapFontName, const Texture2D* texture)
	: m_spriteSheet(new SpriteSheet(texture, 16, 16))
	, m_name(bitmapFontName)
	, m_textureFilePath("Data/Fonts/" + bitmapFontName + ".png")
{
}

BitmapFont::~BitmapFont()
{
	SAFE_DELETE(m_spriteSheet);
}

AABB2 BitmapFont::GetTexCoordsForGlyph(int glyphUnicode) const
{
	return m_spriteSheet->getTexCoordsForSpriteIndex(glyphUnicode);
}

const Texture& BitmapFont::getTexture() const
{
	return m_spriteSheet->getTexture();
}

const Texture2D& BitmapFont::getTexture2D() const
{
	return m_spriteSheet->getTextureDX();
}

const float BitmapFont::getTextWidth(const std::string& text, float height /*= 1.f*/, float aspectRatio /*= 1.f*/)
{
	float width = text.size() * height * aspectRatio;
	return width;
}
