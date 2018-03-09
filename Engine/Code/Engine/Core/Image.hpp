#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <string>

class Image
{
public:
	Image() {}
	Image(const std::string& filePath);
	~Image();

	void CreateBuffer(int bytesPerTexel, int width, int height);

	Rgba getTexel(const IntVector2& texelPos) const;
	void setTexel(const IntVector2& texelPos, const Rgba& color);
public:
	unsigned char* m_texelBytes;
	IntVector2 m_dimensions;
	int m_bytesPerTexel;
	std::string m_filePath;
};