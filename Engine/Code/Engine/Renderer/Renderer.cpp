#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>					// #ToDo: Remove all references to OpenGL
#include <math.h>
#include <gl/GLU.h>
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "ThirdParty/OpenGL/glext.h"
#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "glu32" ) // Link in the OpenGL32.lib static library
//////////////////////////////////////////////////////////////////////////
PFNGLGENBUFFERSPROC		glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC		glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC		glBufferData = nullptr;
PFNGLGENERATEMIPMAPPROC	glGenerateMipmap = nullptr;
PFNGLDELETEBUFFERSPROC	glDeleteBuffers = nullptr;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
Renderer::Renderer()
{
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");

	if(glGenBuffers == nullptr || glBindBuffer == nullptr || glBufferData == nullptr || glGenerateMipmap == nullptr || glDeleteBuffers == nullptr)
		ERROR_AND_DIE("Get a real computer!");

	CreateBuiltInTextures();
	m_genericVBO = CreateAndGetVBOId();
}
//////////////////////////////////////////////////////////////////////////
void Renderer::ClearScreen(const Rgba& clearColor)
{
	float red, green, blue, alpha;
	clearColor.GetAsFloats(red, green, blue, alpha);
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::ClearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::EnableCapability(Capability cap)
{
	glEnable(cap);
}

void Renderer::DisableCapability(Capability cap)
{
	glDisable(cap);
}

void Renderer::BindTexture(const Texture* texture)
{
	EnableCapability(CAPABILITY_TEXTURE_2D);
	if (texture)
	{
		glBindTexture(CAPABILITY_TEXTURE_2D, texture->m_textureID);
	}
	else
	{
		glBindTexture(CAPABILITY_TEXTURE_2D, m_plainWhiteTexture->m_textureID);
	}
}

void Renderer::BindBuffer(unsigned int bufferId, BufferType bufferType /*= BUFFER_ARRAY*/)
{
	glBindBuffer(bufferType, bufferId);
}

void Renderer::EnableBackFaceCulling(bool isNowBackFaceCulling /*= true*/)
{
	if (isNowBackFaceCulling)
	{
		EnableCapability(CAPABILITY_CULL_FACE);
	}
	else
	{
		DisableCapability(CAPABILITY_CULL_FACE);
	}
}

void Renderer::EnableDepthTesting(bool isNowDepthTesting /*= true*/)
{
	if (isNowDepthTesting)
		EnableCapability(CAPABILITY_DEPTH_TEST);
	else
		DisableCapability(CAPABILITY_DEPTH_TEST);
}
//////////////////////////////////////////////////////////////////////////
void Renderer::SetOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight)
{
	glLoadIdentity();
	glOrtho(bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, -1.f, 1.f);
}

void Renderer::SetPerspectiveProjection(float fovVerticalDegrees, float aspectRatio, float nearDistance, float farDistance)
{
	glLoadIdentity();
	gluPerspective(fovVerticalDegrees, aspectRatio, nearDistance, farDistance);
}

void Renderer::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void Renderer::SetLineWidth(float lineThickness)
{
	glLineWidth(lineThickness);
}

void Renderer::SetColor(const Rgba& color)
{
	glColor4ub(color.r, color.g, color.b, color.a);
}

void Renderer::SetBlendFunc(BlendFactor sourceFactor, BlendFactor destinationFactor)
{
	glBlendFunc(sourceFactor, destinationFactor);
}

void Renderer::SetDepthFunc(DepthFunction func)
{
	glDepthFunc(func);
}

void Renderer::LoadMatrix(const float* matrix)
{
	glLoadMatrixf(matrix);
}

void Renderer::SetMatrixMode(MatrixMode mode)
{
	glMatrixMode(mode);
}

void Renderer::SetBufferData(const void * data, int size, UsageType usageDrawHint, BufferType bufferType /*= BUFFER_ARRAY*/)
{
	glBufferData(bufferType, size, data, usageDrawHint);
}

void Renderer::SetVBO3DData(unsigned int vboId, const Vertex3D* vertexArray, int numVertexes, UsageType usageDrawHint, BufferType bufferType /*= BUFFER_ARRAY*/)
{
	BindBuffer(vboId, bufferType);
	int size = numVertexes * sizeof(Vertex3D);
	SetBufferData(vertexArray, size, usageDrawHint, bufferType);
	BindBuffer(0);// Reset it to 0
}

void Renderer::GenerateBuffers(unsigned int* buffers, int numberOfBuffers)
{
	glGenBuffers(numberOfBuffers, buffers);
}

void Renderer::DeleteBuffers(unsigned int* buffers, int numberOfBuffers)
{
	glDeleteBuffers(numberOfBuffers, buffers);
}

void Renderer::DeleteVBO(unsigned int vboId)
{
	unsigned int buffers[] = { vboId };
	DeleteBuffers(buffers, 1);
}

//////////////////////////////////////////////////////////////////////////
void Renderer::pushMatrix()
{
	glPushMatrix();
	
}

void Renderer::popMatrix()
{
	glPopMatrix();
}

void Renderer::translate2D(float x, float y)
{
	glTranslatef(x, y, 0.f);
}

void Renderer::translate2D(const Vector2& pos)
{
	translate2D(pos.x, pos.y);
}

void Renderer::translate3D(const Vector3& pos)
{
	translate3D(pos.x, pos.y, pos.z);
}

void Renderer::translate3D(float x, float y, float z)
{
	glTranslatef(x, y, z);
}

void Renderer::rotate2D(float degrees)
{
	rotate3D(degrees, 0.f, 0.f, 1.f);
}

void Renderer::rotate3D(float degrees, const Vector3& rotationAxis)
{
	rotate3D(degrees,rotationAxis.x, rotationAxis.y, rotationAxis.z);
}

void Renderer::rotate3D(float degrees, float x, float y, float z)
{
	glRotatef(degrees, x, y, z);
}

void Renderer::scale2DUniform(float scale)
{
	scale3DUniform(scale);
}

void Renderer::scale3DUniform(float scale)
{
	Vector3 scales = Vector3::ONE * scale;
	scale3D(scales);
}

void Renderer::scale3D(const Vector3& scales)
{
	glScalef(scales.x, scales.y, scales.z);
}

void Renderer::rotateAroundPosition2D(const Vector2& position, float degrees)
{
	translate2D(position);
	rotate2D(degrees);
	translate2D(position*-1.f);
}
//////////////////////////////////////////////////////////////////////////
void Renderer::drawVertexArray3D(const Vertex3D* vertexArray, int numVertexes, PrimativeType type)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	constexpr int stride = sizeof(Vertex3D);
	glVertexPointer(3, GL_FLOAT, stride, &vertexArray[0].m_pos);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, &vertexArray[0].m_color);
	glTexCoordPointer(2, GL_FLOAT, stride, &vertexArray[0].m_textureCoords);

	glDrawArrays(type, 0, numVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Renderer::drawVBO3D(unsigned int vboID, int numVertexes, PrimativeType type)
{
	drawVBO3D(vboID, numVertexes, type, 0);
}

void Renderer::drawVBO3D(unsigned int vboID, int numVertexes, PrimativeType type, int first)
{
	BindBuffer(vboID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	constexpr int stride = sizeof(Vertex3D);
	const GLvoid* posOffset = (const GLvoid*)offsetof(Vertex3D, m_pos);
	const GLvoid* colorOffset = (const GLvoid*)offsetof(Vertex3D, m_color);
	const GLvoid* textureOffset = (const GLvoid*)offsetof(Vertex3D, m_textureCoords);

	glVertexPointer(3, GL_FLOAT, stride, posOffset);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, colorOffset);
	glTexCoordPointer(2, GL_FLOAT, stride, textureOffset);

	glDrawArrays(type, first, numVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	BindBuffer(0);
}

//////////////////////////////////////////////////////////////////////////
void Renderer::drawLine(const Vector2& startPos, const Vector2& endPos, const Rgba& startColor, const Rgba& endColor)
{
	drawLine(Vector3(startPos.x, startPos.y, Z2D), Vector3(endPos.x, endPos.y, Z2D), startColor, endColor);
}

void Renderer::drawLine(const Vector2& startPos, const Vector2& endPos, const Rgba& color)
{
	drawLine(startPos, endPos, color, color);
}

void Renderer::drawLine(const Vector3& startPos, const Vector3& endPos, const Rgba& startColor, const Rgba& endColor)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(startPos, startColor));
	vertexArray.push_back(Vertex3D(endPos, endColor));

	BindTexture(m_plainWhiteTexture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), PRIMATIVE_LINE);
}

void Renderer::drawLineAndSetLineWidth(const Vector2& startPos, const Vector2& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness)
{
	SetLineWidth(lineThickness);
	drawLine(startPos, endPos, startColor, endColor);
}

void Renderer::drawLineAndSetLineWidth(const Vector3& startPos, const Vector3& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness)
{
	SetLineWidth(lineThickness);
	drawLine(startPos, endPos, startColor, endColor);
}

void Renderer::drawLineStrip(const Vertex2D* vertexes, int numVertexes)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.reserve(numVertexes);
	
	for (int i = 0; i < numVertexes; ++i)
	{
		vertexArray.push_back(vertexes[i]);
	}

	BindTexture(m_plainWhiteTexture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), PRIMATIVE_LINE_STRIP);
}

//////////////////////////////////////////////////////////////////////////
// void Renderer::drawLine(float startX, float startY, float endX, float endY)
// {
// 	glDisable(GL_TEXTURE_2D);
// 	glBegin(GL_LINES);
// 	{
// 		glVertex2f(startX, startY);
// 		glVertex2f(endX, endY);
// 	}
// 	glEnd();
// }
// 
// void Renderer::drawRegularPolygon(float centerX, float centerY, float numSides, float exRadius, float offsetDegrees)
// {
// 	glDisable(GL_TEXTURE_2D);
// 	glBegin(GL_LINE_LOOP);
// 	float degreesPerSide = (2.f * M_PI) / numSides;
// 	float offsetRadians = convertDegreesToRadians(offsetDegrees);
// 	for (int i = 0; i < numSides; i++)
// 	{
// 		float angleTheta = degreesPerSide*i + offsetRadians;
// 
// 		float x = centerX + exRadius * cosf(angleTheta);
// 		float y = centerY + exRadius * sinf(angleTheta);
// 		glVertex2f(x, y);
// 	}
// 	glEnd();
// }
// 
// void Renderer::drawCircle(float centerX, float centerY, float radius, float numSides)
// {
// 	drawRegularPolygon(centerX, centerY, numSides, radius);
// }

//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTexture(const std::string& imageFilePath)
{
	// Try to find that texture from those already loaded
	Texture* texture = GetTexture(imageFilePath);
	if (texture)
		return texture;

	texture = CreateTextureFromFile(imageFilePath);
	return texture;
}


BitmapFont* Renderer::CreateOrGetFont(const std::string& bitmapFontName)
{
	BitmapFont* bitmapFont = GetBitmapFont(bitmapFontName);
	if (bitmapFont)
		return bitmapFont;

	bitmapFont = CreateBitmapFromFile(bitmapFontName);
	return bitmapFont;
}

unsigned int Renderer::CreateAndGetVBOId()
{
	unsigned int vboId[1];
	GenerateBuffers(vboId, 1);
	return vboId[0];
}

//-----------------------------------------------------------------------------------------------
// void Renderer::DrawTexturedAABB2(const AABB2& bounds, const Texture& texture)
// {
// 	SetColor(Rgba::WHITE);
// 	glEnable(GL_TEXTURE_2D);
// 	//glBindTexture(GL_TEXTURE_2D, texture.m_textureID);
// 	glBegin(GL_QUADS);
// 	{
// 		glTexCoord2f(0.f, 1.f);
// 		glVertex2f(bounds.mins.x, bounds.mins.y);
// 
// 		glTexCoord2f(1.f, 1.f);
// 		glVertex2f(bounds.maxs.x, bounds.mins.y);
// 
// 		glTexCoord2f(1.f, 0.f);
// 		glVertex2f(bounds.maxs.x, bounds.maxs.y);
// 
// 		glTexCoord2f(0.f, 0.f);
// 		glVertex2f(bounds.mins.x, bounds.maxs.y);
// 	}
// 	glEnd();
// }

void Renderer::drawTexturedAABB2(const AABB2& bounds, const Texture& texture, const AABB2& textureCoords)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.mins.y, Z2D), Vector2(textureCoords.mins.x, textureCoords.maxs.y)));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.mins.y, Z2D), Vector2(textureCoords.maxs.x, textureCoords.maxs.y)));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.maxs.y, Z2D), Vector2(textureCoords.maxs.x, textureCoords.mins.y)));
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.maxs.y, Z2D), Vector2(textureCoords.mins.x, textureCoords.mins.y)));
	BindTexture(&texture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), PRIMATIVE_QUADS);
}

void Renderer::drawTexturedAABB2(const AABB2& bounds, const Texture& texture, const Rgba& tint)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.mins.y, Z2D), Vector2(0.f, 1.f), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.mins.y, Z2D), Vector2(1.f, 1.f), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.maxs.y, Z2D), Vector2(1.f, 0.f), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.maxs.y, Z2D), Vector2(0.f, 0.f), tint));
	BindTexture(&texture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), PRIMATIVE_QUADS);
}

void Renderer::drawTexturedAABB2(const AABB2& bounds, const AABB2& textureCoords /*= AABB2::ZERO_TO_ONE*/, const Rgba& tint /*= Rgba::WHITE*/)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.mins.y, Z2D), Vector2(textureCoords.mins.x, textureCoords.maxs.y), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.mins.y, Z2D), Vector2(textureCoords.maxs.x, textureCoords.maxs.y), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.maxs.y, Z2D), Vector2(textureCoords.maxs.x, textureCoords.mins.y), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.maxs.y, Z2D), Vector2(textureCoords.mins.x, textureCoords.mins.y), tint));
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), PRIMATIVE_QUADS);
}

void Renderer::drawTexturedSquare(const Vector2& offset, float scale, const AABB2& textureCoords /*= AABB2::ZERO_TO_ONE*/, const Rgba& tint /*= Rgba::WHITE*/)
{
	AABB2 square = AABB2::ZERO_TO_ONE;
	square.maxs *= scale;
	square.translate(offset);
	drawTexturedAABB2(square, textureCoords, tint);
}

void Renderer::drawAABB2(const AABB2& bounds, const Rgba& color)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.mins.y, Z2D), Vector2(0.f, 1.f), color));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.mins.y, Z2D), Vector2(1.f, 1.f), color));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.maxs.y, Z2D), Vector2(1.f, 0.f), color));
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.maxs.y, Z2D), Vector2(0.f, 0.f), color));
	BindTexture(m_plainWhiteTexture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), PRIMATIVE_QUADS);
}


void Renderer::drawAABB2(const Vector2& mins, const Vector2& maxs, const Rgba& color)
{
	drawAABB2(AABB2(mins, maxs), color);
}

void Renderer::drawAABB2(const IntVector2& mins, const IntVector2& maxs, const Rgba& color)
{
	drawAABB2(Vector2((float)mins.x, (float)mins.y), Vector2((float)maxs.x, (float)maxs.y), color);
}

void Renderer::drawDisc(const Vector2& center, float radius, const Rgba& color, float numSides)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.reserve((unsigned int)numSides);
	float angleInRadians = (2.f * M_PI) / numSides;

	vertexArray.push_back(Vertex3D(Vector3(center.x, center.y, Z2D), color));

	for (int i = 0; i <= numSides; i++)
	{
		float angleTheta = angleInRadians*i;

		float x = center.x + radius * cosf(angleTheta);
		float y = center.y + radius * sinf(angleTheta);
		vertexArray.push_back(Vertex3D(Vector3(x, y, Z2D), color));
	}

	BindTexture(m_plainWhiteTexture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), PRIMATIVE_TRIANGLE_FAN);
}
//////////////////////////////////////////////////////////////////////////
void Renderer::drawAxes(float axisLength, float alpha)
{
	//Draw X-axis
	Rgba red;
	red.SetAsFloats(1.0f, 0.0f, 0.0f, alpha);
	drawLine(Vector3::ZERO,Vector3::XAXIS*axisLength, red, red);

	//Draw Y-axis
	Rgba green;
	green.SetAsFloats(0.0f, 1.0f, 0.0f, alpha);
	drawLine(Vector3::ZERO, Vector3::YAXIS*axisLength, green, green);

	//Draw Z-axis
	Rgba blue;
	blue.SetAsFloats(0.0f, 0.0f, 1.0f, alpha);
	drawLine(Vector3::ZERO, Vector3::ZAXIS*axisLength, blue, blue);
}
//////////////////////////////////////////////////////////////////////////
void Renderer::drawQuads(const Vertex3D* vertexes, int numVertexes)
{
	std::vector<Vertex3D> vertexArray;
	for (int vertexIndex = 0; vertexIndex < numVertexes; ++vertexIndex)
	{
		vertexArray.push_back(vertexes[vertexIndex]);
	}

	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), PRIMATIVE_QUADS);
}

void Renderer::drawTexturedQuads(const Vertex3D* vertexes, int numVertexes, const Texture& texture)
{
	BindTexture(&texture);
	drawQuads(vertexes, numVertexes);
}

void Renderer::drawTexturedQuads(const Vertex3D* vertexes, int numVertexes)
{
	drawQuads(vertexes, numVertexes);
}

void Renderer::drawQuads3D(const Vertex3D* vertexArray, int numVertexes)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	constexpr int stride = sizeof(Vertex3D);
	glVertexPointer(3, GL_FLOAT, stride, &vertexArray[0].m_pos);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, &vertexArray[0].m_color);
	glTexCoordPointer(2, GL_FLOAT, stride, &vertexArray[0].m_textureCoords);

	glDrawArrays(GL_QUADS, 0, numVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Renderer::DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint /*= Rgba::WHITE*/, float cellAspectScale /*= 1.f*/, const BitmapFont* font /*= nullptr*/)
{
	if (!font)
	{
		//DebuggerPrintf("Didn't set a font!");
		return;
	}

	std::vector<Vertex3D> vertexArray;
	vertexArray.reserve(asciiText.length() * 4);
	Vector3 bottomLeftStart(startBottomLeft.x, startBottomLeft.y, Z2D);
	float cellWidth = cellHeight * cellAspectScale;

	// Corner offsets
	Vector3 bottomLeftPos(0.f, 0.f, Z2D);
	Vector3 bottomRightPos(cellWidth, 0.f, Z2D);
	Vector3 topRightPos(cellWidth, cellHeight, Z2D);
	Vector3 topLeftPos(0.f, cellHeight, Z2D);

	for (unsigned int letterIndex = 0; letterIndex < asciiText.length(); letterIndex++)
	{
	 	AABB2 letterTexCoords = font->GetTexCoordsForGlyph(asciiText[letterIndex]);
	 	Vector2 bottomLeftTex	(letterTexCoords.mins.x, letterTexCoords.maxs.y);
	 	Vector2 bottomRightTex	(letterTexCoords.maxs.x, letterTexCoords.maxs.y);
	 	Vector2 topRightTex		(letterTexCoords.maxs.x, letterTexCoords.mins.y);
	 	Vector2 topLeftTex		(letterTexCoords.mins.x, letterTexCoords.mins.y);
	 

	 	vertexArray.push_back(Vertex3D((bottomLeftStart + bottomLeftPos), bottomLeftTex, tint));
	 	vertexArray.push_back(Vertex3D((bottomLeftStart + bottomRightPos), bottomRightTex, tint));
	 	vertexArray.push_back(Vertex3D((bottomLeftStart + topRightPos), topRightTex, tint));
	 	vertexArray.push_back(Vertex3D((bottomLeftStart + topLeftPos), topLeftTex, tint));
	 
	 	bottomLeftStart += bottomRightPos;
	}

	BindTexture(&font->getTexture());
// 	for (unsigned int letterIndex = 0; letterIndex < asciiText.length(); letterIndex++)
// 	{
// 		Vector3 mins(bottomLeftPos + bottomLeftStart);
// 		Vector3 maxs(topRightPos + bottomLeftStart);
// 		drawTexturedAABB2(AABB2(Vector2(mins.x, mins.y),Vector2(maxs.x, maxs.y)), font->GetTexCoordsForGlyph(asciiText[letterIndex]), tint);
// 
// 		bottomLeftStart += bottomRightPos;
// 	}
	//drawQuads2D(vertexArray.data(), vertexArray.size());
 	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), USAGE_DYNAMIC_DRAW);
 	drawVBO3D(m_genericVBO, vertexArray.size(), PRIMATIVE_QUADS);
}




//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile(const std::string& imageFilePath)
{
	// Load image data
	int width = 0;
	int height = 0;
	int bytesPerTexel = 0;
	unsigned char* imageTexelBytes = stbi_load(imageFilePath.c_str(), &width, &height, &bytesPerTexel, 0);
	GUARANTEE_OR_DIE(imageTexelBytes != nullptr, Stringf("Failed to load image file \"%s\" - file not found!", imageFilePath.c_str()));
	GUARANTEE_OR_DIE(bytesPerTexel == 3 || bytesPerTexel == 4, Stringf("Failed to load image file \"%s\" - image had unsupported %i bytes per texel (must be 3 or 4)", imageFilePath.c_str(), bytesPerTexel));

	// Create texture on video card, and send image (texel) data
	Texture* texture = CreateTextureFromData(imageFilePath, imageTexelBytes, width, height, bytesPerTexel);
	stbi_image_free(imageTexelBytes);

	return texture;
}


Texture* Renderer::CreateTextureFromData(const std::string& name, unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel)
{
	unsigned int openGLTextureID = CreateOpenGLTexture(imageTexelBytes, width, height, bytesPerTexel);
	// Create (new) a Texture object
	Texture* texture = new Texture();
	texture->m_textureID = openGLTextureID;
	texture->m_imageFilePath = name;
	texture->m_texelDimensions.setXY(width, height);

	m_alreadyLoadedTextures.push_back(texture);
	return texture;
}

void Renderer::CreateBuiltInTextures()
{
	unsigned char plainWhiteTexel[4] = { 255, 255, 255, 255 };
	m_plainWhiteTexture = CreateTextureFromData("White", plainWhiteTexel, 1, 1, 4);
}

//-----------------------------------------------------------------------------------------------
Texture* Renderer::GetTexture(const std::string& imageFilePath)
{
	for (int textureIndex = 0; textureIndex < (int)m_alreadyLoadedTextures.size(); ++textureIndex)
	{
		Texture* texture = m_alreadyLoadedTextures[textureIndex];
		if (imageFilePath == texture->m_imageFilePath)
			return texture;
	}

	return nullptr;
}


//-----------------------------------------------------------------------------------------------
unsigned int Renderer::CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel)
{
	// Create a texture ID (or "name" as OpenGL calls it) for this new texture
	unsigned int openGLTextureID = 0xFFFFFFFF;
	glGenTextures(1, &openGLTextureID);

	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture(GL_TEXTURE_2D, openGLTextureID);

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT

																  // Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if (bytesPerTexel == 3)
		bufferFormat = GL_RGB;

	GLenum internalFormat = bufferFormat; // the format we want the texture to me on the card; allows us to translate into a different texture format as we upload to OpenGL

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		width,				// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
		height,				// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
		imageTexelBytes);	// Location of the actual pixel data bytes/buffer

	return openGLTextureID;
}

BitmapFont* Renderer::CreateBitmapFromFile(const std::string& bitmapFontName)
{
	Texture* texture = CreateOrGetTexture("Data/Fonts/" + bitmapFontName + ".png");

	BitmapFont* font = new BitmapFont(bitmapFontName, *texture);
	m_loadedFonts.insert(std::pair<std::string, BitmapFont*>(bitmapFontName,font));
	return font;
}

BitmapFont* Renderer::GetBitmapFont(const std::string& bitmapFontName)
{
	auto search = m_loadedFonts.find(bitmapFontName);

	if(search == m_loadedFonts.end())
		return nullptr;
	else
	{
		return m_loadedFonts.at(bitmapFontName);
	}
}


