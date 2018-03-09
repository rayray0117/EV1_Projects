#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"
//////////////////////////////////////////////////////////////////////////
class Texture2D;
//////////////////////////////////////////////////////////////////////////
class SpriteSheet
{
public:
	SpriteSheet(const Texture& texture, int tilesWide, int tilesHigh);
	SpriteSheet(const Texture2D* texture, int tilesWide, int tilesHigh);
	~SpriteSheet();

	AABB2 getTexCoordsForSpriteCoords(int spriteX, int spriteY) const; // mostly for atlases
	AABB2 getTexCoordsForSpriteIndex(int spriteIndex) const; // mostly for sprite animations
	int getNumSprites() const;
	const Texture& getTexture() const;
	const Texture2D& getTextureDX() const;

private:
	const Texture* 	m_spriteSheetTexture;	// Texture with grid-based layout of sub-textures
	const Texture2D* m_spriteSheetTextureDX;
	IntVector2	m_spriteLayout;		// # of sprites across, and down, on the sheet
									//... and other data member variables as needed
};

