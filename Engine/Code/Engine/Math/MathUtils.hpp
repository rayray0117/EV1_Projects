#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
// Regular Common Math numbers
constexpr float M_PI	   = 3.1415926535897932384626433832795f;
constexpr float M_E        = 2.71828182845904523536f;	// e
constexpr float M_LOG2E    = 1.44269504088896340736f;   // log2(e)
constexpr float M_LOG10E   = 0.434294481903251827651f;  // log10(e)
constexpr float M_LN2      = 0.693147180559945309417f;  // ln(2)
constexpr float M_LN10     = 2.30258509299404568402f;   // ln(10)
constexpr float M_PI_2     = 1.57079632679489661923f;   // pi/2
constexpr float M_PI_4     = 0.785398163397448309616f;  // pi/4
constexpr float M_1_PI     = 0.318309886183790671538f;  // 1/pi
constexpr float M_2_PI     = 0.636619772367581343076f;  // 2/pi
constexpr float M_2_SQRTPI = 1.12837916709551257390f;   // 2/sqrt(pi)
constexpr float M_SQRT2    = 1.41421356237309504880f;   // sqrt(2)
constexpr float M_SQRT1_2  = 0.707106781186547524401f; // 1/sqrt(2)
// Digital Storage Size numbers
constexpr float M_KiB_I	   = 0.0009765625f;
constexpr float M_MiB_I     = 0.00000095367431640625f;
constexpr float M_GiB_I     = 0.000000000931322574615478515625f;

namespace EV1
{
	constexpr float RAD2DEG = 57.29578f;
	constexpr float DEG2RAD = 0.0174532924f;

	template<typename T>
	T Min(const T& left, const T& right)
	{
		if (left < right)
			return left;

		return right;
	}

	template<typename T>
	T Max(const T& left, const T& right)
	{
		if (left > right)
			return left;

		return right;
	}
}
//-
//Required for A1
float convertRadiansToDegrees(float radians);
float convertDegreesToRadians(float degrees);
int getRandomIntLessThan(int maxValueNotInclusive);
int getRandomIntInRange(int minValueInclusive, int maxValueInclusive);
float getRandomFloatZeroToOne();
float getRandomFloatLessThan(float maxValueInclusive);
float getRandomFloatInRange(float minimumInclusive, float maximumInclusive);
int getRandomNegativeOrPositiveOne();

template<typename T>
T clamp(const T& value, const T& minVal, const T& maxVal)
{
	if (value <= minVal)
		return minVal;
	if (value >= maxVal)
		return maxVal;
	return value;
}

float rangeMapFloat(float inputValue, float inputMin, float inputMax, float outputMin, float outputMax);

void turnToward(float& currentDegrees, float goalDegrees, float maxTurnDegrees);
float calcShortestAngularDistance(float startDegress, float endDegrees);

FORCEINLINE float sign(float value)
{
	if (value >= 0)
	{
		return 1.f;
	}
	return -1.f;
}
//-
// Bonus Stuff for me
bool isPercentChance(float);
float cosDegrees(float degrees);
float sinDegrees(float degrees);
float inverse_f(float divisor);
double inverse_d(double divisor);
float inverseSqrt_f(float divisor);
float convertToFloat(int integer);
float convertToFloat(double doubleNumber);
float convertToFloat(short shortNumber);
float atan2Degrees(float y, float x);
float smoothStart(float t, int power);
float smoothStop(float t, int power);
float SmoothStep3(float t);
float interpolate(float start, float end, float fractionToEnd);

//Got idea for trunc functions from UE4
FORCEINLINE int truncToInt(float f) { return (int)f; }
FORCEINLINE float truncToFloat(float f) { return (float)truncToInt(f); }

namespace UE4 //This namespace is to cover functions that are borrowed from UE4 for weird math that I don't quite understand yet.
{
	FORCEINLINE void SinCos(float& scalarSin, float& scalarCos, float angleRadians)
	{
		// Using UE4's equation because the math for Quaternions is weird and confusing.
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float quotient = (M_1_PI*0.5f)*angleRadians;
		if (angleRadians >= 0.0f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient - 0.5f));
		}
		float y = angleRadians - (2.0f*M_PI)*quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > M_PI_2)
		{
			y = M_PI - y;
			sign = -1.0f;
		}
		else if (y < -M_PI_2)
		{
			y = -M_PI - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float y2 = y * y;

		// 11-degree minimax approximation
		scalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		scalarCos = sign*p;
	}

	/**
	* Returns the floating-point remainder of X / Y
	* Warning: Always returns remainder toward 0, not toward the smaller multiple of Y.
	*			So for example Fmod(2.8f, 2) gives .8f as you would expect, however, Fmod(-2.8f, 2) gives -.8f, NOT 1.2f
	* Use Floor instead when snapping positions that can be negative to a grid
	*/
	float FloatMod(float X, float Y);
}

inline uint to1DMatixValues(uint row, uint column)
{
	return (row * 4) + column;
}

bool areMostlyEqual(float a, float b, float tolerance = 0.001f);
//////////////////////////////////////////////////////////////////////////
// Inlines
inline float convertRadiansToDegrees(float radians)
{
	return radians * EV1::RAD2DEG;
}

inline float convertDegreesToRadians(float degrees)
{
	return degrees * EV1::DEG2RAD;
}

inline float rangeMapFloat(float inputValue, float inputMin, float inputMax, float outputMin, float outputMax)
{
	// x						[a,b]
	// x-a						[0,b-a]
	// (x-a)/(b-a)				[0,1]
	// ((x-a)/(b-a))(d-c)		[0,d-c]
	// ((x-a)/(b-a))(d-c) + c	[c,d]
	float fractionBit = (inputValue - inputMin) / (inputMax - inputMin);
	return (fractionBit * (outputMax - outputMin)) + outputMin;
}


inline float convertToFloat(int integer)
{
	return static_cast<float>(integer);
}

inline float convertToFloat(double doubleNumber)
{
	return static_cast<float>(doubleNumber);
}

inline float convertToFloat(short shortNumber)
{
	return static_cast<float>(shortNumber);
}

