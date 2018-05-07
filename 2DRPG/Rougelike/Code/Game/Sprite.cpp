#include "Game/Sprite.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
//////////////////////////////////////////////////////////////////////////
Sprite::Sprite(char glyph, const Rgba& glyphColor, const Rgba& fillColor)
	: m_glyph(glyph)
	, m_glyphColor(glyphColor)
	, m_fillColor(fillColor)
{
}
//////////////////////////////////////////////////////////////////////////
void Sprite::drawFill(const IntVector2& tileCoords) const
{
    g_renderer->drawAABB2(tileCoords, tileCoords + IntVector2::ONE, m_fillColor);
}

void Sprite::drawGlyph(const IntVector2& tileCoords) const
{
    //Do this because string is too stupid to construct with one character;
    std::string str = "";
    str += m_glyph;
    g_renderer->DrawText2D(Vector2((float)tileCoords.x, (float)tileCoords.y), str, 1.0, m_glyphColor, 1.0f, g_renderer->CreateOrGetBitmapFont(g_spriteBitmap));
}

void Sprite::drawGlyphOverride(const IntVector2& tileCoords, char overrideGlyph) const
{
	//Do this because string is too stupid to construct with one character;
	std::string str = "";
	str += overrideGlyph;
	g_renderer->DrawText2D(Vector2((float)tileCoords.x, (float)tileCoords.y), str, 1.0, m_glyphColor, 1.0f, g_renderer->CreateOrGetBitmapFont(g_spriteBitmap));
}

void Sprite::drawOverlay(const IntVector2& tileCoords, const Rgba& overlayColor) const
{
	g_renderer->drawAABB2(tileCoords, tileCoords + IntVector2::ONE, overlayColor);
}

void Sprite::redrawFill(const IntVector2& tileCoords) const
{
	MeshBuilder& mb = *g_theGame->m_fills;
	mb.setColor(m_fillColor);
	AABB2 uv = AABB2::ZERO_TO_ONE;
	mb.addVertex(Vector3((float)tileCoords.x + 0.f, (float)tileCoords.y + 0.f, .0f), Vector2(uv.mins.x, uv.maxs.y));
	mb.addVertex(Vector3((float)tileCoords.x + 1.f, (float)tileCoords.y + 0.f, .0f), Vector2(uv.maxs.x, uv.maxs.y));
	mb.addVertex(Vector3((float)tileCoords.x + 1.f, (float)tileCoords.y + 1.f, .0f), Vector2(uv.maxs.x, uv.mins.y));
	mb.addVertex(Vector3((float)tileCoords.x + 0.f, (float)tileCoords.y + 1.f, .0f), Vector2(uv.mins.x, uv.mins.y));
}

void Sprite::redrawGlyph(const IntVector2& tileCoords) const
{
	MeshBuilder& mb = *g_theGame->m_glyphs;
	mb.setColor(m_glyphColor);
	BitmapFont* font = g_renderer->CreateOrGetBitmapFont(g_spriteBitmap);
	AABB2 uv = font->GetTexCoordsForGlyph(m_glyph);
	mb.addVertex(Vector3((float)tileCoords.x + 0.f, (float)tileCoords.y + 0.f, .0f), Vector2(uv.mins.x, uv.maxs.y));
	mb.addVertex(Vector3((float)tileCoords.x + 1.f, (float)tileCoords.y + 0.f, .0f), Vector2(uv.maxs.x, uv.maxs.y));
	mb.addVertex(Vector3((float)tileCoords.x + 1.f, (float)tileCoords.y + 1.f, .0f), Vector2(uv.maxs.x, uv.mins.y));
	mb.addVertex(Vector3((float)tileCoords.x + 0.f, (float)tileCoords.y + 1.f, .0f), Vector2(uv.mins.x, uv.mins.y));
}

void Sprite::redrawGlyphOverride(const IntVector2& tileCoords, char overrideGlyph) const
{
	MeshBuilder& mb = *g_theGame->m_glyphs;
	mb.setColor(m_glyphColor);
	BitmapFont* font = g_renderer->CreateOrGetBitmapFont(g_spriteBitmap);
	AABB2 uv = font->GetTexCoordsForGlyph(overrideGlyph);
	mb.addVertex(Vector3((float)tileCoords.x + 0.f, (float)tileCoords.y + 0.f, .0f), Vector2(uv.mins.x, uv.maxs.y));
	mb.addVertex(Vector3((float)tileCoords.x + 1.f, (float)tileCoords.y + 0.f, .0f), Vector2(uv.maxs.x, uv.maxs.y));
	mb.addVertex(Vector3((float)tileCoords.x + 1.f, (float)tileCoords.y + 1.f, .0f), Vector2(uv.maxs.x, uv.mins.y));
	mb.addVertex(Vector3((float)tileCoords.x + 0.f, (float)tileCoords.y + 1.f, .0f), Vector2(uv.mins.x, uv.mins.y));
}

void Sprite::redrawOverlay(const IntVector2& tileCoords, const Rgba& overlayColor) const
{
	MeshBuilder& mb = *g_theGame->m_overlays;
	mb.setColor(overlayColor);
	AABB2 uv = AABB2::ZERO_TO_ONE;
	mb.addVertex(Vector3((float)tileCoords.x + 0.f, (float)tileCoords.y + 0.f, .0f), Vector2(uv.mins.x, uv.maxs.y));
	mb.addVertex(Vector3((float)tileCoords.x + 1.f, (float)tileCoords.y + 0.f, .0f), Vector2(uv.maxs.x, uv.maxs.y));
	mb.addVertex(Vector3((float)tileCoords.x + 1.f, (float)tileCoords.y + 1.f, .0f), Vector2(uv.maxs.x, uv.mins.y));
	mb.addVertex(Vector3((float)tileCoords.x + 0.f, (float)tileCoords.y + 1.f, .0f), Vector2(uv.mins.x, uv.mins.y));
}
