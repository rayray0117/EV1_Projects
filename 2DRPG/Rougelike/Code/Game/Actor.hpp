//This is what Squirel would call an entity. But I use UE4 too much to want to change naming conventions.
#pragma once
#include <string>

#include "Game/Sprite.hpp"
#include "Game/Stats.hpp"
//////////////////////////////////////////////////////////////////////////
class Map;
class Tile;
class Inventory;
class Item;
//////////////////////////////////////////////////////////////////////////
class Actor
{
public:
	Actor() {}
	Actor(char glyph, const Rgba& glyphColor, const Rgba& fillColor);
	virtual ~Actor();

	virtual void Update(float deltaSeconds);
	virtual void Render() const;
	virtual void DebugRender() const;
	virtual void Redraw();

	void DropInventory();
	void DropItem(uint itemToDropIndex);


	IntVector2 getTileCoords() const;

	void drawFill() const;
	void drawGlyph() const;

	const bool isDead() const;

	const bool operator==(const std::string& typeName) const;
	const bool operator!=(const std::string& typeName) const;
public:
	std::string m_name;
	Map* mo_currentMap = nullptr;
	Tile* mo_currentTile = nullptr;
	Inventory* m_inventory = nullptr;
    Sprite m_sprite;

	//Combat variables
	Stats m_baseStats;
public: 
	//Battle stuff
	int currentHP;
	int currentStamina;
};
//////////////////////////////////////////////////////////////////////////
// Inlines
inline const bool Actor::operator==(const std::string& typeName) const
{
	return m_name == typeName;
}

inline const bool Actor::operator!=(const std::string& typeName) const
{
	return m_name != typeName;
}