#pragma once
#include "Engine/Core/Rgba.hpp"
//////////////////////////////////////////////////////////////////////////
class IntVector2;
//////////////////////////////////////////////////////////////////////////
class Sprite
{
public:
	Sprite() {}
	Sprite(char glyph, const Rgba& glyphColor, const Rgba& fillColor);
	~Sprite() {}

    void drawFill(const IntVector2& tileCoords) const;
    void drawGlyph(const IntVector2& tileCoords) const;
	void drawGlyphOverride(const IntVector2& tileCoords, char overrideGlyph) const;
	void drawOverlay(const IntVector2& tileCoords, const Rgba& overlayColor) const;
	void redrawFill(const IntVector2& tileCoords) const;
	void redrawGlyph(const IntVector2& tileCoords) const;
	void redrawGlyphOverride(const IntVector2& tileCoords, char overrideGlyph) const;
	void redrawOverlay(const IntVector2& tileCoords, const Rgba& overlayColor) const;
public:
	char m_glyph;
	Rgba m_glyphColor;
	Rgba m_fillColor;
};