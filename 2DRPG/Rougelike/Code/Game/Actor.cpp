#include "Game/Actor.hpp"
#include "Engine/Core/EngineBase.hpp"

#include "Game/Inventory.hpp"
#include "Game/Tile.hpp"
//////////////////////////////////////////////////////////////////////////
Actor::Actor(char glyph, const Rgba& glyphColor, const Rgba& fillColor)
    : m_sprite(glyph, glyphColor, fillColor)
{
}

Actor::~Actor()
{
	DropInventory();
}
//////////////////////////////////////////////////////////////////////////
void Actor::Update(float /*deltaSeconds*/)
{
    /* DO NOTHING */
}

void Actor::Render() const
{
    m_sprite.drawGlyph(mo_currentTile->m_tileCoords);
}

void Actor::DebugRender() const
{

}

void Actor::Redraw()
{
	m_sprite.redrawGlyph(mo_currentTile->m_tileCoords);
}

//////////////////////////////////////////////////////////////////////////
void Actor::DropInventory()
{
	if (m_inventory == nullptr)
		return;

	if (!mo_currentTile)
	{
		SAFE_DELETE(m_inventory);
		return;
	}


	if (mo_currentTile->m_inventory)
	{
		*mo_currentTile->m_inventory += *m_inventory;
		SAFE_DELETE(m_inventory);
		//m_inventory = nullptr; // the tile owns that memory now so make it so we don't have it.
	}
	else
	{
		mo_currentTile->m_inventory = m_inventory;
		m_inventory = nullptr; // the tile owns that memory now so make it so we don't have it.
	}
}

void Actor::DropItem(uint itemToDropIndex)
{
	Item* droppedItem = m_inventory->RemoveItem(itemToDropIndex);
	mo_currentTile->m_inventory->AddItem(droppedItem);
}
//////////////////////////////////////////////////////////////////////////
IntVector2 Actor::getTileCoords() const
{
	return mo_currentTile->m_tileCoords;
}

void Actor::drawFill() const
{
	m_sprite.drawFill(mo_currentTile->m_tileCoords);
}

void Actor::drawGlyph() const
{
	m_sprite.drawGlyph(mo_currentTile->m_tileCoords);
}

const bool Actor::isDead() const
{
	return currentHP <= 0;
}

