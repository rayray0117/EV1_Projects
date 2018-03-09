#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Math/MathUtils.hpp"
//////////////////////////////////////////////////////////////////////////
const UIntVector4 UIntVector4::ZERO(0, 0, 0, 0);
const UIntVector4 UIntVector4::ONE(1, 1, 1, 1);
//////////////////////////////////////////////////////////////////////////
UIntVector4::UIntVector4(const UIntVector4& copy)
	:x(copy.x)
	, y(copy.y)
	, z(copy.z)
	, w(copy.w)
{
}

UIntVector4::UIntVector4(uint initialX, uint initialY, uint initialZ, uint initialW)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
	, w(initialW)
{
}
//////////////////////////////////////////////////////////////////////////
const uint* UIntVector4::getAsArray() const
{
	return &x;
}

uint* UIntVector4::getAsArray()
{
	return &x;
}
//////////////////////////////////////////////////////////////////////////
const UIntVector4 UIntVector4::operator+(const UIntVector4& rightHandSide) const
{
	return UIntVector4(x + rightHandSide.x, y + rightHandSide.y, z + rightHandSide.z, w + rightHandSide.w);
}

const UIntVector4 UIntVector4::operator-(const UIntVector4& rightHandSide) const
{
	return UIntVector4(x - rightHandSide.x, y - rightHandSide.y, z - rightHandSide.z, w - rightHandSide.w);
}

// const UIntVector4 UIntVector4::operator*(float scale) const
// {
// 	return UIntVector4(x * scale, y * scale, z * scale, w * scale);
// }
// 
// const UIntVector4 UIntVector4::operator*(const UIntVector4& perAxisScaleFactors) const
// {
// 	return UIntVector4(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z, w * perAxisScaleFactors.w);
// }
// 
// const UIntVector4 UIntVector4::operator/(float inverseScale)
// {
// 	float scale = inverse_f(inverseScale);
// 	return UIntVector4(x * scale, y * scale, z * scale, w * scale);
// }

void UIntVector4::operator-=(const UIntVector4& rightHandSide)
{
	x -= rightHandSide.x;
	y -= rightHandSide.y;
	z -= rightHandSide.z;
	w -= rightHandSide.w;
}

void UIntVector4::operator+=(const UIntVector4& rightHandSide)
{
	x += rightHandSide.x;
	y += rightHandSide.y;
	z += rightHandSide.z;
	w += rightHandSide.w;
}

// void UIntVector4::operator*=(const UIntVector4& perAxisScaleFactors)
// {
// 	x *= perAxisScaleFactors.x;
// 	y *= perAxisScaleFactors.y;
// 	z *= perAxisScaleFactors.z;
// 	w *= perAxisScaleFactors.w;
// }
// 
// void UIntVector4::operator*=(float scale)
// {
// 	x *= scale;
// 	y *= scale;
// 	z *= scale;
// 	w *= scale;
// }

bool UIntVector4::operator!=(const UIntVector4& rightHandSide) const
{
	return((x != rightHandSide.x) || (y != rightHandSide.y) || (z != rightHandSide.z) || (w != rightHandSide.w)); // equivalent to !((x == rightHandSide.x) && (y == rightHandSide.y) && (z == rightHandSide.z));
}

bool UIntVector4::operator==(const UIntVector4& rightHandSide) const
{
	return ((x == rightHandSide.x) && (y == rightHandSide.y) && (z == rightHandSide.z) && (w == rightHandSide.w));
}

