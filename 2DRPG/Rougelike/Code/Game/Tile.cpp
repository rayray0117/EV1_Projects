#include "Tile.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Game/Inventory.hpp"
#include "Game/Feature.hpp"
#include "Game/Pawn.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba.hpp"
//////////////////////////////////////////////////////////////////////////
Tile::Tile(const IntVector2& tileCoords, char glyph, const Rgba& glyphColor, const Rgba& fillColor)
    : m_sprite(glyph, glyphColor, fillColor)
    , m_tileCoords(tileCoords)
{
}

Tile::~Tile()
{
    SAFE_DELETE(m_inventory);
}
//////////////////////////////////////////////////////////////////////////
void Tile::Render() const
{
	if (m_visibilty == TILE_VIS_NEVER_SEEN)
		return;

    m_sprite.drawFill(m_tileCoords);
	if (m_visibilty == TILE_VIS_WAS_SEEN)
	{
		m_sprite.drawGlyph(m_tileCoords);
		m_sprite.drawOverlay(m_tileCoords, Rgba(Rgba::BLACK, .75f));
		return;
	}

    if (mo_pawn)
    {
        mo_pawn->Render();
    }
    else if (mo_feature)
    {
        mo_feature->Render();
    }
    else if (m_inventory)
    {
        m_sprite.drawGlyphOverride(m_tileCoords, '*');
    }
    else
    {
        m_sprite.drawGlyph(m_tileCoords);
    }
}

void Tile::DebugRender() const
{
	g_renderer->pushMatrix();
	BitmapFont* f = g_renderer->CreateOrGetBitmapFont(g_bitmapFont);
	g_renderer->translate2D(Vector2(0.f, -20.f));
	g_renderer->DrawText2D(Vector2(0.f, 0.f), Stringf("%s tile. (%i, %i)", mo_tileDef->m_name.c_str(), m_tileCoords.x, m_tileCoords.y), 20.f, Rgba::WHITE, 1.f, f);
	g_renderer->DrawText2D(Vector2(0.f, -15.f), Stringf("IsSolid: %s", boolToString(mo_tileDef->m_isSolid).c_str()), 15.f, Rgba::WHITE, 1.f, f);
	g_renderer->translate2D(Vector2(0.f, -40.f));

	if (mo_feature)
		mo_feature->DebugRender();
	if (mo_pawn)
		mo_pawn->DebugRender();
	if (m_inventory)
		m_inventory->DebugRender();
	g_renderer->popMatrix();
}

void Tile::Redraw()
{
	if (m_visibilty == TILE_VIS_NEVER_SEEN)
		return;

	m_sprite.redrawFill(m_tileCoords);
	if (m_visibilty == TILE_VIS_WAS_SEEN)
	{
		m_sprite.redrawGlyph(m_tileCoords);
		m_sprite.redrawOverlay(m_tileCoords, Rgba(Rgba::BLACK, .75f));
		return;
	}

	if (mo_pawn)
	{
		mo_pawn->Redraw();
	}
	else if (mo_feature)
	{
		mo_feature->Redraw();
	}
	else if (m_inventory)
	{
		m_sprite.redrawGlyphOverride(m_tileCoords, '*');
	}
	else
	{
		m_sprite.redrawGlyph(m_tileCoords);
	}
}
//////////////////////////////////////////////////////////////////////////
void Tile::changeType(TileDefinition* tileDef)
{
    mo_tileDef = tileDef;
    m_sprite.m_glyph = tileDef->m_glyphs[getRandomIntLessThan(tileDef->m_glyphs.size())];
    m_sprite.m_glyphColor = tileDef->m_glyphColors[getRandomIntLessThan(tileDef->m_glyphColors.size())];
    m_sprite.m_fillColor = tileDef->m_fillColors[getRandomIntLessThan(tileDef->m_fillColors.size())];
}

void Tile::changeType(const std::string& tileTypeName)
{
	//If I sent an empty string then I probably am generating stuff and want to keep the tile the same rather than make it null.
	//Also if I really want it null I'd just use the tile pointer version rather than text.
	if (tileTypeName.empty())
		return;

    changeType(TileDefinition::getDefinition(tileTypeName));
}

bool Tile::isOpen() const
{
    if (mo_pawn)
        return false;

    if (mo_feature && mo_feature->m_isSolid)
        return false;

    return !mo_tileDef->m_isSolid;
}

bool Tile::isSolid() const
{
	return mo_tileDef->m_isSolid;
}

bool Tile::isOpaque() const
{
	return mo_tileDef->m_isOpaque;
}

void Tile::drawIsSelectableOverlay(const Rgba& color) const
{
	m_sprite.drawOverlay(m_tileCoords, color);
}

float Tile::getBaseGCost()
{
	if (mo_tileDef->m_isSolid)
		return 100.f;

	return 1.f; //Just say it is one for now if not solid
}

const bool Tile::operator==(const std::string& tileTypeName) const
{
	return tileTypeName == mo_tileDef->m_name;
}

const bool Tile::operator!=(const std::string& tileTypeName) const
{
	return tileTypeName != mo_tileDef->m_name;
}
