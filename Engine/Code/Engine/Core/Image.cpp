#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
#include "ThirdParty/stb/stb_image.h"
//////////////////////////////////////////////////////////////////////////
Image::Image(const std::string& filePath)
{
	// Load image data
	m_dimensions.x = 0;
	m_dimensions.y = 0;
	m_bytesPerTexel = 0;
	m_texelBytes = stbi_load(filePath.c_str(), &m_dimensions.x, &m_dimensions.y, &m_bytesPerTexel, 4);
}

Image::~Image()
{
	stbi_image_free(m_texelBytes);
}

void Image::CreateBuffer(int bytesPerTexel, int width, int height)
{
	m_dimensions.x = width;
	m_dimensions.y = height;
	m_bytesPerTexel = bytesPerTexel;

	m_texelBytes = new unsigned char[width*height*bytesPerTexel];
}

Rgba Image::getTexel(const IntVector2& texelPos) const
{
	int index = texelPos.x + texelPos.y * m_dimensions.x;
	int byteOffset = index * m_bytesPerTexel;

	Rgba color;
	color.r = m_texelBytes[byteOffset];
	color.g = m_texelBytes[byteOffset + 1];
	color.b = m_texelBytes[byteOffset + 2];
	if (m_bytesPerTexel == 4)
		color.a = m_texelBytes[byteOffset + 3];
	else
		color.a = 255;

	return color;
}

void Image::setTexel(const IntVector2& texelPos, const Rgba& color)
{
	int index = texelPos.x + texelPos.y * m_dimensions.x;
	int byteOffset = index * m_bytesPerTexel;
	const uchar* colorArray = &color.r;
	for (int i = 0; i < m_bytesPerTexel; i++)
	{
		m_texelBytes[byteOffset + i] = colorArray[i];
	}
}
