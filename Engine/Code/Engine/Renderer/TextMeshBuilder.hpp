#pragma once
#include "Engine/Renderer/MeshBuilder.hpp"
//////////////////////////////////////////////////////////////////////////
class Font;
class BitmapFont;
//////////////////////////////////////////////////////////////////////////
class TextMeshBuilder : public MeshBuilder
{
public:
	TextMeshBuilder() : MeshBuilder() {}
	
	void begin(Font* fontToUse);
	void begin(BitmapFont* bitmapFontToUse);

	void setCursorPosition(const Vector2& position);

	void addString(const std::string& str);
	void addLetter(char letter);
public:
	Font* mo_fontToUse = nullptr;
	BitmapFont* mo_bitmapFontToUse = nullptr;

	Vector2 m_cursorPos;
	float m_scale;
	float m_aspectRatio;
private:
	void addStringBitmap(const std::string& str);
};