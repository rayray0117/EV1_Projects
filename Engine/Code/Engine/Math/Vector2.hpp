#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class IntVector2;
class Vector3;
//////////////////////////////////////////////////////////////////////////
class Vector2
{
public:
	float x;
	float y;

	static const Vector2 ZERO;
	static const Vector2 ONE;
	static const Vector2 XAXIS;
	static const Vector2 YAXIS;

public:
	Vector2();
	Vector2(const Vector2& copy);
	explicit Vector2(float initialX, float initialY);
	Vector2(const IntVector2& copy);
	Vector2(const Vector3& copy);

	const float* getAsFloatArray() const;
	float* getAsFloatArray();


	//Accessors
	void getXY(float& outX, float& outY) const;
	float calcLength() const;
	float calcLengthSquared() const;
	float calcHeadingDegrees() const;
	float calcHeadingRadians() const;
	Vector2 getNormalizedVector() const;
	

	//Mutators
	void setXY(float newX, float newY);
	void rotate90Degrees();
	void rotateNegative90Degrees();
	void rotateDegrees(float degrees);
	void rotateRadians(float radians);
	float normalize(); // Will return the vector's initial length
	float setLength(float newLength); // Will return the vector's initial length
	void scaleUniform(float scale);
	void scaleNonUniform(const Vector2& perAxisScaleFactors);
	void inverseScaleNonUniform(const Vector2& perAxisScaleDivisors);
	void setHeadingDegrees(float headingDegrees);
	void setHeadingRadians(float headingRadians);
	void setUnitLengthAndHeadingDegrees(float headingDegrees);
	void setUnitLengthAndHeadingRadians(float headingRadians);
	void setLengthAndHeadingDegrees(float newLength, float headingDegrees);
	void setLengthAndHeadingRadians(float newLength, float headingRadians);

	// Operators
	bool operator==(const Vector2& rightHandSide) const;
	bool operator!=(const Vector2& rightHandSide) const;
	const Vector2 operator+(const Vector2& rightHandSide) const;
	const Vector2 operator-(const Vector2& rightHandSide) const;
	const Vector2 operator-() const;
	const Vector2 operator*(float scale) const;
	const Vector2 operator*(const Vector2& perAxisScaleFactors) const;
	const Vector2 operator/(float inverseScale) const;
	void operator*=(float scale);
	void operator*=(const Vector2& perAxisScaleFactors);
	void operator+=(const Vector2& rightHandSide);
	void operator-=(const Vector2& rightHandSide);

	bool isMostlyEqualTo(const Vector2& comparison, float epsilon = 0.001f);

	//Standalone friend helper functions
	friend float calcDistance(const Vector2& posA, const Vector2& posB);
	friend float calcDistanceSquared(const Vector2& posA, const Vector2& posB);
	friend const Vector2 operator*(float scale, const Vector2& vectorToScale);
	friend bool areMostlyEqual(const Vector2& a, const Vector2& b, float epsilon = 0.001f);
	friend Vector2 bounceVector(const Vector2& in, const Vector2& normal);
	friend const Vector2 interpolate(const Vector2& start, const Vector2& end, float fractionToEnd);
	friend const Vector2 rangeMap(const Vector2& inputValue, Vector2 inputMin, Vector2 inputMax, Vector2 outputMin, Vector2 outputMax);

	// Bonus Stuff for me
	void rotate180Degrees();
};

float dotProduct(const Vector2& a, const Vector2& b);
Vector2 projectAlongVector(const Vector2& a, const Vector2& b);
//////////////////////////////////////////////////////////////////////////
// Inlines
//////////////////////////////////////////////////////////////////////////
// Constructors
inline Vector2::Vector2()
{
}

inline Vector2::Vector2(const Vector2& copy)
	: x(copy.x)
	, y(copy.y)
{
}
inline Vector2::Vector2(float initialX, float initialY)
	: x(initialX)
	, y(initialY)
{
}
//////////////////////////////////////////////////////////////////////////
// Accessoers
inline float Vector2::calcLengthSquared() const
{
	return (x * x) + (y * y);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
inline bool Vector2::operator==(const Vector2& rightHandSide) const
{
	return ((x == rightHandSide.x) && (y == rightHandSide.y));
}

inline bool Vector2::operator!=(const Vector2& rightHandSide) const
{
	return ((x != rightHandSide.x) || (y != rightHandSide.y)); // equivalent to !((x == rightHandSide.x) && (y == rightHandSide.y));
}

inline const Vector2 Vector2::operator+(const Vector2& rightHandSide) const
{
	return Vector2(x + rightHandSide.x, y + rightHandSide.y);
}

inline const Vector2 Vector2::operator-(const Vector2& rightHandSide) const
{
	return Vector2(x - rightHandSide.x, y - rightHandSide.y);
}

inline const Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

inline const Vector2 Vector2::operator*(float scale) const
{
	return Vector2(x * scale, y * scale);
}

inline const Vector2 Vector2::operator*(const Vector2& perAxisScaleFactors) const
{
	return Vector2(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y);
}

inline void Vector2::operator*=(float scale)
{
	x *= scale;
	y *= scale;
}

inline void Vector2::operator*=(const Vector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

inline void Vector2::operator+=(const Vector2& rightHandSide)
{
	x += rightHandSide.x;
	y += rightHandSide.y;
}

inline void Vector2::operator-=(const Vector2& rightHandSide)
{
	x -= rightHandSide.x;
	y -= rightHandSide.y;
}
//////////////////////////////////////////////////////////////////////////
// "friend" functions
inline const Vector2 operator*(float scale, const Vector2& vectorToScale)
{
	return vectorToScale * scale;
}

inline float calcDistanceSquared(const Vector2& posA, const Vector2& posB)
{
	Vector2 displacement = posA - posB;
	return displacement.calcLengthSquared();
}

inline float dotProduct(const Vector2& a, const Vector2& b)
{
	return ((a.x * b.x) + (a.y * b.y));
}