#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"
//////////////////////////////////////////////////////////////////////////
// Pure Colors
const Rgba Rgba::BLACK(0, 0, 0, 255);
const Rgba Rgba::WHITE(255, 255, 255, 255);
const Rgba Rgba::RED(255, 0, 0, 255);
const Rgba Rgba::GREEN(0, 255, 0, 255);
const Rgba Rgba::BLUE(0, 0, 255, 255);
const Rgba Rgba::GRAY(128, 128, 128, 255);
const Rgba Rgba::YELLOW(255, 255, 0, 255);
const Rgba Rgba::MAGENTA(255, 0, 255, 255);
const Rgba Rgba::CYAN(0, 255, 255, 255);
// Mid Colors
const Rgba Rgba::GRAY_64(64, 64, 64, 255);
const Rgba Rgba::GRAY_192(192, 192, 192, 255);

// Special Custom Colors
const Rgba Rgba::SKY_DAYLIGHT(178, 216, 255, 255);
const Rgba Rgba::SKY_MOONLIGHT(0, 24, 72, 255);

//The Casey collection
const Rgba Rgba::FORESTGREEN(34, 139, 34, 255);
const Rgba Rgba::NAVY_BLUE(0, 0, 128, 255);
const Rgba Rgba::ORANGE(255, 165, 0, 255);
const Rgba Rgba::VIOLET(143, 0, 255, 255);
const Rgba Rgba::OLIVE(107, 142, 35, 255);
const Rgba Rgba::SKY_BLUE(45, 185, 255, 255);
const Rgba Rgba::LIME(227, 255, 0, 255);
const Rgba Rgba::TEAL(0, 128, 128, 255);
const Rgba Rgba::TURQUOISE(64, 224, 208, 255);
const Rgba Rgba::PERIWINKLE(204, 204, 255, 255);
const Rgba Rgba::NORMAL_Z(128, 128, 255, 255);

Rgba Rgba::Random() 
{
	return Rgba
	(
		static_cast<unsigned char>(getRandomIntLessThan(256)),
		static_cast<unsigned char>(getRandomIntLessThan(256)),
		static_cast<unsigned char>(getRandomIntLessThan(256)),
		255
	);
}
//////////////////////////////////////////////////////////////////////////
Rgba::Rgba()
{
}

Rgba::Rgba(const Rgba& copy)
	: r(copy.r)
	, g(copy.g)
	, b(copy.b)
	, a(copy.a)
{
}

Rgba::Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha /*= 255*/)
	: r(red)
	, g(green)
	, b(blue)
	, a(alpha)
{
}

Rgba::Rgba(const Rgba& copy, float alpha)
	: r(copy.r)
	, g(copy.g)
	, b(copy.b)
	, a(convertColorFloatToUnsignedChar(alpha))
{
}

Rgba::~Rgba()
{
}

void Rgba::SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte /*= 255*/)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void Rgba::SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha /*= 1.0f*/)
{
	r = convertColorFloatToUnsignedChar(normalizedRed);
	g = convertColorFloatToUnsignedChar(normalizedGreen);
	b = convertColorFloatToUnsignedChar(normalizedBlue);
	a = convertColorFloatToUnsignedChar(normalizedAlpha);
}


void Rgba::GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const
{
	out_normalizedRed = convertColorUnsignedCharToFloat(r);
	out_normalizedGreen = convertColorUnsignedCharToFloat(g);
	out_normalizedBlue = convertColorUnsignedCharToFloat(b);
	out_normalizedAlpha = convertColorUnsignedCharToFloat(a);
}

void Rgba::ScaleRGB(float rgbScale)
{
	float red, green, blue, alpha;
	GetAsFloats(red, green, blue, alpha);

	red = clamp(red * rgbScale, 0.f, 1.f);
	green = clamp(green * rgbScale, 0.f, 1.f);
	blue = clamp(blue * rgbScale, 0.f, 1.f);

	SetAsFloats(red, green, blue, alpha);
}

void Rgba::ScaleAlpha(float alphaScale)
{
	float alpha = convertColorUnsignedCharToFloat(a);
	alpha = clamp(alpha * alphaScale, 0.f, 1.f);
	a = convertColorFloatToUnsignedChar(alpha);
}

bool Rgba::operator==(const Rgba& rightHandSide) const
{
	return(r == rightHandSide.r && g == rightHandSide.g && b == rightHandSide.b && a == rightHandSide.a);
}

const Rgba interpolate(const Rgba& start, const Rgba& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	float startRed, startGreen, startBlue, startAlpha;
	float endRed, endGreen, endBlue, endAlpha;
	start.GetAsFloats(startRed, startGreen, startBlue, startAlpha);
	end.GetAsFloats(endRed, endGreen, endBlue, endAlpha);

	Rgba blended;
	float red = (fractionOfStart * startRed) + (fractionToEnd * endRed);
	float green = (fractionOfStart * startGreen) + (fractionToEnd * endGreen);
	float blue = (fractionOfStart * startBlue) + (fractionToEnd * endBlue);
	float alpha = (fractionOfStart * startAlpha) + (fractionToEnd * endAlpha);
	blended.SetAsFloats(red, green, blue, alpha);
	return blended;
}

unsigned char Rgba::convertColorFloatToUnsignedChar(float inputValue) const
{
	return static_cast<unsigned char>(inputValue * 255);
}

float Rgba::convertColorUnsignedCharToFloat(unsigned char inputValue) const
{
	return static_cast<float>(inputValue) / static_cast<float>(255);
}
