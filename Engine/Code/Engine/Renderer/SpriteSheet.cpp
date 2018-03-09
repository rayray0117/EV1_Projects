#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/RHI/Texture2D.hpp"

SpriteSheet::SpriteSheet(const Texture& texture, int tilesWide, int tilesHigh)
	: m_spriteSheetTexture(&texture)
	, m_spriteLayout(tilesWide, tilesHigh)
	, m_spriteSheetTextureDX(nullptr)
{
}

SpriteSheet::SpriteSheet(const Texture2D* texture, int tilesWide, int tilesHigh)
	: m_spriteSheetTextureDX(texture)
	, m_spriteLayout(tilesWide, tilesHigh)
	, m_spriteSheetTexture(nullptr)
{
}

SpriteSheet::~SpriteSheet()
{
}

AABB2 SpriteSheet::getTexCoordsForSpriteCoords(int spriteX, int spriteY) const
{
	float x = convertToFloat(spriteX);
	float y = convertToFloat(spriteY);
	Vector2 mins(x / m_spriteLayout.x, y / m_spriteLayout.y);
	Vector2 maxs((x + 1.f) / m_spriteLayout.x, (y + 1.f) / m_spriteLayout.y);
	return  AABB2(mins, maxs);
}

AABB2 SpriteSheet::getTexCoordsForSpriteIndex(int spriteIndex) const
{
	int y = spriteIndex / m_spriteLayout.x;
	int x = spriteIndex % m_spriteLayout.x;
	return getTexCoordsForSpriteCoords(x, y);
}

int SpriteSheet::getNumSprites() const
{
	return m_spriteLayout.x * m_spriteLayout.y;
}

const Texture& SpriteSheet::getTexture() const
{
	return *m_spriteSheetTexture;
}

const Texture2D& SpriteSheet::getTextureDX() const
{
	return *m_spriteSheetTextureDX;
}
