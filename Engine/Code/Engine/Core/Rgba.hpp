#pragma once


//////////////////////////////////////////////////////////////////////////
class Rgba
{
public:
	unsigned char r; // Red byte, 0-255
	unsigned char g; // Green byte, 0-255
	unsigned char b; // Blue byte, 0-255
	unsigned char a; // Alpha (opacity) byte, 0-255; 0 is transparent, 255 is opaque

	// Pure colors
	static const Rgba BLACK;
	static const Rgba WHITE;
	static const Rgba RED;
	static const Rgba GREEN;
	static const Rgba BLUE;
	static const Rgba GRAY;
	static const Rgba YELLOW;
	static const Rgba MAGENTA;
	static const Rgba CYAN;

	// Mid colors
	static const Rgba GRAY_64;
	static const Rgba GRAY_192;

	// Special Custom Colors
	static const Rgba SKY_DAYLIGHT;
	static const Rgba SKY_MOONLIGHT;

	// The Casey collection;
	static const Rgba FORESTGREEN;
	static const Rgba NAVY_BLUE;
	static const Rgba ORANGE;
	static const Rgba VIOLET;
	static const Rgba OLIVE;
	static const Rgba SKY_BLUE;
	static const Rgba LIME;
	static const Rgba TEAL;
	static const Rgba TURQUOISE;
	static const Rgba PERIWINKLE;
	static const Rgba NORMAL_Z;
	static Rgba Random();

public:
	Rgba();
	Rgba(const Rgba& copy);
	Rgba(const Rgba& copy, float alpha);
	explicit Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);
	~Rgba();


	void SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255);
	void SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha = 1.0f);
	void GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const;
	void ScaleRGB(float rgbScale); // Scales (and clamps) RGB components, but not A
	void ScaleAlpha(float alphaScale); // Scales (and clamps) Alpha, RGB is untouched etc.

	bool operator==(const Rgba& rightHandSide) const;

	friend const Rgba interpolate(const Rgba& start, const Rgba& end, float fractionToEnd);
private:
	unsigned char convertColorFloatToUnsignedChar(float inputValue) const;
	float convertColorUnsignedCharToFloat(unsigned char inputValue) const;
};