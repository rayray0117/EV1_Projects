#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector3.hpp"
//////////////////////////////////////////////////////////////////////////
const Vector4 Vector4::ZERO(0.f, 0.f, 0.f, 0.f);
const Vector4 Vector4::ONE(1.f, 1.f, 1.f, 1.f);
//////////////////////////////////////////////////////////////////////////
Vector4::Vector4()
{
}

Vector4::Vector4(const Vector4& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
	, w(copy.w)
{
}

Vector4::Vector4(float initialX, float initialY, float initialZ, float initialW)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
	, w(initialW)
{
}

Vector4::Vector4(const Rgba& colors)
{
	colors.GetAsFloats(x, y, z, w);
}

Vector4::Vector4(const Rgba& colors, float intensity)
	: w(intensity)
{
	colors.GetAsFloats(x, y, z, intensity);
}

Vector4::Vector4(const Vector3& copy, float _w)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
	, w(_w)
{
}

const float* Vector4::getAsFloatArray() const
{
	return &x;
}

float* Vector4::getAsFloatArray()
{
	return &x;
}
//////////////////////////////////////////////////////////////////////////
// Accessors
void Vector4::getXYZW(float& outX, float& outY, float& outZ, float& outW) const
{
	outX = x;
	outY = y;
	outZ = z;
	outW = w;
}

float Vector4::calcLength3D() const
{
	return sqrtf(calcLengthSquared3D());
}

float Vector4::calcLengthSquared3D() const
{
	return (x * x) + (y * y) + (z * z);
}

float Vector4::calcLength4D() const
{
	return sqrtf(calcLengthSquared4D());
}

float Vector4::calcLengthSquared4D() const
{
	return (x * x) + (y * y) + (z * z) + (w * w);
}
//////////////////////////////////////////////////////////////////////////
// Mutators
void Vector4::setXYZW(float newX, float newY, float newZ, float newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

float Vector4::normalize3D()
{
	float length = calcLength3D();

	scaleUniform(inverse_f(length));
	return length;
}

float Vector4::normalize4D()
{
	float length = calcLength4D();

	scaleUniform(inverse_f(length));
	return length;
}

void Vector4::scaleUniform(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void Vector4::scaleNonUniform(const Vector4& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

void Vector4::inverseScaleNonUniform(const Vector4& perAxisScaleDivisors)
{
	x *= inverse_f(perAxisScaleDivisors.x);
	y *= inverse_f(perAxisScaleDivisors.y);
	z *= inverse_f(perAxisScaleDivisors.z);
	w *= inverse_f(perAxisScaleDivisors.w);
}

bool Vector4::operator==(const Vector4& rightHandSide) const
{
	return ((x == rightHandSide.x) && (y == rightHandSide.y) && (z == rightHandSide.z) && (w == rightHandSide.w));
}

//////////////////////////////////////////////////////////////////////////
// Operators


void Vector4::operator*=(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void Vector4::operator*=(const Vector4& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

const Vector4 Vector4::operator*(float scale) const
{
	return Vector4(x * scale, y * scale, z * scale, w * scale);
}

const Vector4 Vector4::operator*(const Vector4& perAxisScaleFactors) const
{
	return Vector4(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z, w * perAxisScaleFactors.w);
}

float Vector4::MaxAbsoluteValue()
{
	float result = 0.f;
	float checkVal;

	checkVal = x;
	if ((checkVal > result))
		result = checkVal;
	else if ((-checkVal > result))
		result = -checkVal;

	checkVal = y;
	if ((checkVal > result))
		result = checkVal;
	else if((-checkVal > result))
		result = -checkVal;

	checkVal = z;
	if ((checkVal > result))
		result = checkVal;
	else if ((-checkVal > result))
		result = -checkVal;

	checkVal = w;
	if ((checkVal > result))
		result = checkVal;
	else if ((-checkVal > result))
		result = -checkVal;

	return result;
}

const Vector4 Vector4::operator/(float inverseScale)
{
	float scale = inverse_f(inverseScale);
	return Vector4(x * scale, y * scale, z * scale, w * scale);
}

bool Vector4::operator!=(const Vector4& rightHandSide) const
{
	return((x != rightHandSide.x) || (y != rightHandSide.y) || (z != rightHandSide.z) ||  (w != rightHandSide.w)); // equivalent to !((x == rightHandSide.x) && (y == rightHandSide.y) && (z == rightHandSide.z));
}

void Vector4::operator+=(const Vector4& rightHandSide)
{
	x += rightHandSide.x;
	y += rightHandSide.y;
	z += rightHandSide.z;
	w += rightHandSide.w;
}

const Vector4 Vector4::operator+(const Vector4& rightHandSide) const
{
	return Vector4(x + rightHandSide.x, y + rightHandSide.y, z + rightHandSide.z, w + rightHandSide.w);
}

void Vector4::operator-=(const Vector4& rightHandSide)
{
	x -= rightHandSide.x;
	y -= rightHandSide.y;
	z -= rightHandSide.z;
	w -= rightHandSide.w;
}

const Vector4 Vector4::operator-(const Vector4& rightHandSide) const
{
	return Vector4(x - rightHandSide.x, y - rightHandSide.y, z - rightHandSide.z, w - rightHandSide.w);
}
//////////////////////////////////////////////////////////////////////////
bool Vector4::isMostlyEqualTo(const Vector4& comparison, float epsilon /*= 0.001f*/)
{
	float diffX = x - comparison.x;
	float diffY = y - comparison.y;
	float diffZ = z - comparison.z;
	float diffW = w - comparison.w;
	return (((diffX < epsilon) && (-diffX < epsilon)) && ((diffY < epsilon) && (-diffY < epsilon)) && ((diffZ < epsilon) && (-diffZ < epsilon)) && ((diffW < epsilon) && (-diffW < epsilon)));
}

// float calcDistance(const Vector4& posA, const Vector4& posB)
// {
// 
// }
// 
// float calcDistanceSquared(const Vector4& posA, const Vector4& posB)
// {
// 
// }

const Vector4 operator*(float scale, const Vector4& vectorToScale)
{
	return vectorToScale * scale;
}

bool areMostlyEqual(const Vector4& a, const Vector4& b, float epsilon)
{
	float diffX = a.x - b.x;
	float diffY = a.y - b.y;
	float diffZ = a.z - b.z;
	float diffW = a.w - b.w;
	return (((diffX < epsilon) && (-diffX < epsilon)) && ((diffY < epsilon) && (-diffY < epsilon)) && ((diffZ < epsilon) && (-diffZ < epsilon)) && ((diffW < epsilon) && (-diffW < epsilon)));
}

float dotProduct(const Vector4& a, const Vector4& b)
{
	return ((a.x * b.x) + (a.y * b.y) * (a.z + b.z) * (a.w + b.w));
}

const Vector4 interpolate(const Vector4& start, const Vector4& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	Vector4 blended;
	blended.x = (fractionOfStart * start.x) + (fractionToEnd * end.x);
	blended.y = (fractionOfStart * start.y) + (fractionToEnd * end.y);
	blended.z = (fractionOfStart * start.z) + (fractionToEnd * end.z);
	blended.w = (fractionOfStart * start.w) + (fractionToEnd * end.w);
	return blended;
}