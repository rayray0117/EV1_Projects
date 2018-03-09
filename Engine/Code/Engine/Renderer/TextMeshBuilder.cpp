#include "Engine/Renderer/TextMeshBuilder.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
//////////////////////////////////////////////////////////////////////////
void TextMeshBuilder::begin(Font* fontToUse)
{
	MeshBuilder::begin(RHIEnum::PRIMATIVE_QUADS, false);
	mo_fontToUse = fontToUse;
	mo_bitmapFontToUse = nullptr;
}

void TextMeshBuilder::begin(BitmapFont* bitmapFontToUse)
{
	MeshBuilder::begin(RHIEnum::PRIMATIVE_QUADS, false);
	mo_fontToUse = nullptr;
	mo_bitmapFontToUse = bitmapFontToUse;
}

void TextMeshBuilder::setCursorPosition(const Vector2& position)
{
	m_cursorPos =position;
}

void TextMeshBuilder::addString(const std::string& /*str*/)
{
	if (mo_bitmapFontToUse)
	{

	}

}

void TextMeshBuilder::addStringBitmap(const std::string& /*str*/)
{
// 	Vector3 bottomLeftStart(m_cursorPos.x, m_cursorPos.y, Z2D);
// 	float cellHeight = m_scale;
// 	float cellWidth = cellHeight * m_aspectRatio;
// 
// 	// Corner offsets
// 	Vector3 bottomLeftPos(0.f, 0.f, Z2D);
// 	Vector3 bottomRightPos(cellWidth, 0.f, Z2D);
// 	Vector3 topRightPos(cellWidth, cellHeight, Z2D);
// 	Vector3 topLeftPos(0.f, cellHeight, Z2D);
// 
// 	for (unsigned int letterIndex = 0; letterIndex < str.length(); letterIndex++)
// 	{
// 		AABB2 letterTexCoords = mo_bitmapFontToUse->GetTexCoordsForGlyph(str[letterIndex]);
// 		Vector2 bottomLeftTex(letterTexCoords.mins.x, letterTexCoords.maxs.y);
// 		Vector2 bottomRightTex(letterTexCoords.maxs.x, letterTexCoords.maxs.y);
// 		Vector2 topRightTex(letterTexCoords.maxs.x, letterTexCoords.mins.y);
// 		Vector2 topLeftTex(letterTexCoords.mins.x, letterTexCoords.mins.y);
// 
// 
// 		vertexArray.push_back(Vertex3D((bottomLeftStart + bottomLeftPos), bottomLeftTex, str.m_color));
// 		vertexArray.push_back(Vertex3D((bottomLeftStart + bottomRightPos), bottomRightTex, str.m_color));
// 		vertexArray.push_back(Vertex3D((bottomLeftStart + topRightPos), topRightTex, str.m_color));
// 		vertexArray.push_back(Vertex3D((bottomLeftStart + topLeftPos), topLeftTex, str.m_color));
// 
// 		bottomLeftStart += bottomRightPos;
// 	}
}
