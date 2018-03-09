#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"
//////////////////////////////////////////////////////////////////////////
const Vector2 Vector2::ZERO(0.f, 0.f);
const Vector2 Vector2::ONE(1.f, 1.f);
const Vector2 Vector2::XAXIS(1.f, 0.f);
const Vector2 Vector2::YAXIS(0.f, 1.f);
//////////////////////////////////////////////////////////////////////////
// Constructors

Vector2::Vector2(const IntVector2& copy)
	: x((float)copy.x)
	, y((float)copy.y)
{
}

Vector2::Vector2(const Vector3& copy)
	: x(copy.x)
	, y(copy.y)
{
}

//////////////////////////////////////////////////////////////////////////
const float* Vector2::getAsFloatArray() const
{
	return &x;
}

float* Vector2::getAsFloatArray()
{
	return &x;
}

//////////////////////////////////////////////////////////////////////////
// Accessors
void Vector2::getXY(float& outX, float& outY) const
{
	outX = x;
	outY = y;
}

float Vector2::calcLength() const
{
	return sqrtf(calcLengthSquared());
}

float Vector2::calcHeadingDegrees() const
{
	return convertRadiansToDegrees(atan2f(y, x));
}

float Vector2::calcHeadingRadians() const
{
	return atan2f(y, x);
}

Vector2 Vector2::getNormalizedVector() const
{
	return Vector2(x, y)/calcLength();
}

//////////////////////////////////////////////////////////////////////////
// Mutators
void Vector2::setXY(float newX, float newY)
{
	x = newX;
	y = newY;
}

void Vector2::rotate90Degrees()
{
	float oldX = x;
	x = -y;
	y = oldX;
}

void Vector2::rotateNegative90Degrees()
{
	float oldX = x;
	x = y;
	y = -oldX;
}

void Vector2::rotateDegrees(float degrees)
{
	rotateRadians(convertDegreesToRadians(degrees));
}

void Vector2::rotateRadians(float radians)
{
	float angle = calcHeadingRadians();
	setHeadingRadians(angle + radians);
}

float Vector2::normalize()
{
	float length = calcLength();

	scaleUniform(inverse_f(length));
	return length;
}

float Vector2::setLength(float newLength)
{
	float initialLength = normalize();
	scaleUniform(newLength);
	return initialLength;
}

void Vector2::scaleUniform(float scale)
{
	x *= scale;
	y *= scale;
}

void Vector2::scaleNonUniform(const Vector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

void Vector2::inverseScaleNonUniform(const Vector2& perAxisScaleDivisors)
{
	x *= inverse_f(perAxisScaleDivisors.x);
	y *= inverse_f(perAxisScaleDivisors.y);
}

void Vector2::setHeadingDegrees(float headingDegrees)
{
	setHeadingRadians(convertDegreesToRadians(headingDegrees));
}

void Vector2::setHeadingRadians(float headingRadians)
{
	float length = calcLength();
	x = length * cosf(headingRadians);
	y = length * sinf(headingRadians);
}

void Vector2::setUnitLengthAndHeadingDegrees(float headingDegrees)
{
	setUnitLengthAndHeadingRadians(convertDegreesToRadians(headingDegrees));
}

void Vector2::setUnitLengthAndHeadingRadians(float headingRadians)
{
	x = cosf(headingRadians);
	y = sinf(headingRadians);
}

void Vector2::setLengthAndHeadingDegrees(float newLength, float headingDegrees)
{
	setLengthAndHeadingRadians(newLength, convertDegreesToRadians(headingDegrees));
}

void Vector2::setLengthAndHeadingRadians(float newLength, float headingRadians)
{
	x = newLength * cosf(headingRadians);
	y = newLength * sinf(headingRadians);
}

//////////////////////////////////////////////////////////////////////////
// Operators
const Vector2 Vector2::operator/(float inverseScale) const
{
	float scale = inverse_f(inverseScale);
	return Vector2(x * scale, y * scale);
}

bool Vector2::isMostlyEqualTo(const Vector2& comparison, float epsilon /*= 0.001f*/)
{
	float diffX = x - comparison.x;
	float diffY = y - comparison.y;
	return (((diffX < epsilon) && (-diffX < epsilon)) && ((diffY < epsilon) && (-diffY < epsilon)));
}

//////////////////////////////////////////////////////////////////////////
// "friend" functions
float calcDistance(const Vector2& posA, const Vector2& posB)
{
	Vector2 displacement = posA - posB;
	return displacement.calcLength();
}

bool areMostlyEqual(const Vector2& a, const Vector2& b, float epsilon /*= 0.001f*/)
{
	float diffX = a.x - b.x;
	float diffY = a.y - b.y;
	return (((diffX < epsilon) && (-diffX < epsilon)) && ((diffY < epsilon) && (-diffY < epsilon)));
}

// Vector2 bounceVector(const Vector2& in, const Vector2& normal)
// {
// 	// #ToDo: implement this for realz;
// 	return Vector2::ZERO;
// }

const Vector2 interpolate(const Vector2& start, const Vector2& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	Vector2 blended;
	blended.x = (fractionOfStart * start.x) + (fractionToEnd * end.x);
	blended.y = (fractionOfStart * start.y) + (fractionToEnd * end.y);
	return blended;
}

Vector2 projectAlongVector(const Vector2& a, const Vector2& b)
{
	return (dotProduct(a,b)/dotProduct(b,b))*b;
}

const Vector2 rangeMap(const Vector2& inputValue, Vector2 inputMin, Vector2 inputMax, Vector2 outputMin, Vector2 outputMax)
{
	float x = rangeMapFloat(inputValue.x, inputMin.x, inputMax.x, outputMin.x, outputMax.x);
	float y = rangeMapFloat(inputValue.y, inputMin.y, inputMax.y, outputMin.y, outputMax.y);
	
	return Vector2(x, y);
}

// Bonus
void Vector2::rotate180Degrees()
{
	x = -x;
	y = -y;
}