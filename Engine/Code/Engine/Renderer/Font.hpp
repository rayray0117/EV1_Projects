#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine\Math\AABB2.hpp"
//////////////////////////////////////////////////////////////////////////
struct FontCharacter
{
	int id;
	unsigned int x;
	unsigned int y;
	float width;
	float height;
	float xoffset;
	float yoffset;
	float xadvance;
	unsigned int page;
};

class Font
{
	friend class SimpleRenderer;
//private:
public:
	Font(const std::string& filePath);
	~Font() {};

	void loadFont(const std::string& filePath);

	float calcTextWidth(const std::string& text, float scale = 1.0f);

	AABB2 GetTexCoordsForGlyph(int glyphUnicode) const;

public:
	unsigned int m_lineHeight;
private:
	void getFontCharacters(unsigned int& pos, const std::vector<unsigned char>& buffer, int numChars);
	void getKernings(unsigned int& pos, const std::vector<unsigned char>& buffer, int numKerns);
	
	//FontCharacter Stuff
	const FontCharacter getGlyph(int text) const;
	const FontCharacter getInvalidGlyph() const;
	const float getKerning(int prevCharId, int nextCharId) const;
private:
	std::string m_texturePath;
	std::string m_filePath;
	
	std::map<int, FontCharacter> m_chars;
	std::map<std::pair<int, int>, float> m_kernings;

	std::string m_name; // Face
	unsigned int m_size;
	bool m_bold;
	bool m_italic;

	//unsigned int m_lineHeight;
	float m_divideByLineHeight;
	float m_base;
	unsigned int m_scaleW, m_scaleH;
	unsigned int m_pages;

};
/*
void SimpleRenderer::DrawText(Font* font, int start_x, int start_y, const char* text)
{
	vec2 cursor(start_x,start_y);

	const char* c = text;
	int prev_char_code = -1;

	while (*c != NULL)
	{
		const FontCharacter* glygh = font->get_glyph(*c);

		if (glygh == nullptr)
		{
			glygh = font->get_invalid_glyph();
		}

		// Draw this guy
		Vector2 bottom_left;
		Vector2 top_right;

		Vector2 top_left = cursor + Vector2(glygh->xoffset, -glygh->yoffset);
		bottom_left = top_left + Vector2(0, -glygh->height);
		top_right = top_left + Vector2(glygh->width, 0);
		bottom_right = bottom_left + Vector2(glygh->width, 0);

		Vector2 uv_top_left = Vector2(glygh->x/font->m_scaleW, glygh->y / font->m_scaleH);
		Vector2 uv_bottom_left = uv_top_left + Vector2(0, glygh->height / font->m_scaleH);
		Vector2	uv_top_right = uv_top_left + Vector2(glygh->width / font->m_scaleW, 0);

		top_right = 
		draw_quad2d(bottom_left, top_right, uv_bottom_left, uv_top_right);

		cursor += glygh->xadvance;
		prev_char_code = *c;
		++c;

		int kerning = font->get_kerning(prev_char_code, *c);
		cursor += kerning;
	}
}
*/