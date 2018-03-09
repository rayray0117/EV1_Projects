#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
//////////////////////////////////////////////////////////////////////////
const IntVector3 IntVector3::ZERO(0, 0, 0);
const IntVector3 IntVector3::ONE(1, 1, 1);
const IntVector3 IntVector3::XAXIS(1, 0, 0);
const IntVector3 IntVector3::YAXIS(0, 1, 0);
const IntVector3 IntVector3::ZAXIS(0, 0, 1);
//////////////////////////////////////////////////////////////////////////
// Constructors
IntVector3::IntVector3(Vector3 copy)
	: x((int)floorf(copy.x))
	, y((int)floorf(copy.y))
	, z((int)floorf(copy.z))
{
}

//////////////////////////////////////////////////////////////////////////
const int* IntVector3::getAsIntArray() const
{
	return &x;
}

int* IntVector3::getAsIntArray()
{
	return &x;
}

//////////////////////////////////////////////////////////////////////////
// Accessors
void IntVector3::getXY(int& outX, int& outY, int& outZ) const
{
	outX = x;
	outY = y;
	outZ = z;
}

//////////////////////////////////////////////////////////////////////////
// Mutator
void IntVector3::setXY(int newX, int newY, int newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

//////////////////////////////////////////////////////////////////////////
// Operators

const IntVector3 IntVector3::operator/(float inverseScale)
{
	float scale = inverse_f(inverseScale);
	return IntVector3((int)(x * scale), (int)(y * scale), (int)(z * scale));
}

