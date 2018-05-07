#pragma once
#include "Engine/Math/IntVector2.hpp"
#include <string>
#include "Game/Sprite.hpp"
//////////////////////////////////////////////////////////////////////////
class TileDefinition;
class Inventory;
class Pawn;
class Feature;
//////////////////////////////////////////////////////////////////////////
enum TileVisableState
{
	TILE_VIS_WAS_SEEN,
	TILE_VIS_NEVER_SEEN,
	TILE_VIS_IS_SEEN,
	NUM_TILE_VIS_STATES
};
//////////////////////////////////////////////////////////////////////////
class Tile
{
public:
	Tile() {}
	Tile(const IntVector2& tileCoords, char glyph, const Rgba& glyphColor, const Rgba& fillColor);
	~Tile();

    void Render() const;
	void DebugRender() const;
	void Redraw();

	void changeType(TileDefinition* tileDef);
	void changeType(const std::string& tileTypeName);

    bool isOpen() const;
	bool isSolid() const;
	bool isOpaque() const;

	void drawIsSelectableOverlay(const Rgba& color) const; //For player controller to highlight the selected tiles easily.

	const bool operator==(const std::string& tileTypeName) const;
	const bool operator!=(const std::string& tileTypeName) const;
	float getBaseGCost();
public:
	IntVector2 m_tileCoords;
	TileDefinition* mo_tileDef = nullptr;
	Inventory* m_inventory = nullptr;
	Pawn* mo_pawn = nullptr;
	Feature* mo_feature = nullptr;
    Sprite m_sprite;
	int m_isClosedInPathID = 0;
	int m_isOpenInPathID = 0;
	TileVisableState m_visibilty = TILE_VIS_IS_SEEN;
	std::string m_faction;
};

//////////////////////////////////////////////////////////////////////////
//Inlines 
