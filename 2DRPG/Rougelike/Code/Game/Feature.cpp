#include "Game/Feature.hpp"
#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Inventory.hpp"

//////////////////////////////////////////////////////////////////////////
Feature::Feature()
	: Actor()
{
}

Feature::Feature(std::string defName)
	: Feature()
{
	mo_def = FeatureDefinition::getDefinition(defName);
	m_isSolid = mo_def->m_isSolid;
	m_sprite = mo_def->m_sprite;
	m_name = mo_def->m_name;
	m_isExit = mo_def->m_isExit;
}

Feature::~Feature()
{
	Actor::~Actor();
}

void Feature::Render() const
{
    //Because it is a chest I want to use the fill color
    m_sprite.drawFill(mo_currentTile->m_tileCoords);
    Actor::Render();
}

void Feature::DebugRender() const
{
	g_renderer->DrawText2D(Vector2(0.f, 0.f), Stringf("Feature name: %s", mo_def->m_name.c_str()), 20.f, Rgba::WHITE, 1.f, g_renderer->CreateOrGetBitmapFont(g_bitmapFont));
	g_renderer->translate2D(Vector2(0.f, -20.f));
	if (m_inventory)
		m_inventory->DebugRender();
}

void Feature::Redraw()
{
	//Because it is a chest I want to use the fill color
	m_sprite.redrawFill(mo_currentTile->m_tileCoords);
	Actor::Redraw();
}
