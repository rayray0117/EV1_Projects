#pragma once
#include "Engine/Math/AABB2.hpp"
#include <string>
#include "Engine/Renderer/SpriteSheet.hpp"
//////////////////////////////////////////////////////////////////////////
class Texture2D;
//////////////////////////////////////////////////////////////////////////
class BitmapFont
{
	friend class SimpleRenderer;
	friend class Renderer;
public:
	BitmapFont(const std::string& bitmapFontName, const Texture& texture);
	~BitmapFont();

	AABB2 GetTexCoordsForGlyph(int glyphUnicode) const; // pass ‘A’ or 65 for A, etc.
	//float GetGlyphAspect(int glyphUnicode) { return 1.0f; } // used more later
	const Texture& getTexture() const;
	const Texture2D& getTexture2D() const;

	static const float getTextWidth(const std::string& text, float height = 1.f, float aspectRatio = 1.f);
private:
	//BitmapFont(const std::string& bitmapFontName); // must be constructed by Renderer
	BitmapFont(const std::string& bitmapFontName, const Texture2D* texture);
	std::string m_name;
	std::string m_textureFilePath;
	SpriteSheet* m_spriteSheet; // used internally; assumed to be a 16x16 glyph sheet
};

