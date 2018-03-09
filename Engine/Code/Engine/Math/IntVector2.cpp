#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
//////////////////////////////////////////////////////////////////////////
const IntVector2 IntVector2::ZERO(0, 0);
const IntVector2 IntVector2::ONE(1, 1);
const IntVector2 IntVector2::XAXIS(1, 0);
const IntVector2 IntVector2::YAXIS(0, 1);
//////////////////////////////////////////////////////////////////////////
IntVector2::IntVector2(const Vector2& copy)
	: x((int)floor(copy.x))
	, y((int)floor(copy.y))
{
}
//////////////////////////////////////////////////////////////////////////
const int* IntVector2::getAsIntArray() const
{
	return &x;
}

int* IntVector2::getAsIntArray()
{
	return &x;
}

//////////////////////////////////////////////////////////////////////////
// Accessors
void IntVector2::getXY(int& outX, int& outY) const
{
	outX = x;
	outY = y;
}

//////////////////////////////////////////////////////////////////////////
// Mutator
void IntVector2::setXY(int newX, int newY)
{
	x = newX;
	y = newY;
}

const IntVector2 IntVector2::getRandomDirectionToward(const IntVector2& destination) const
{
	if (destination == *this)
		return IntVector2::ZERO;
	
	// Move in a random direction towards the player
	int axisToMove;
	if (destination.x == x)
	{
		axisToMove = 1; //Move along Y axis
	}
	else if (destination.y == y)
	{
		axisToMove = 0; //Move along X axis
	}
	else
	{
		axisToMove = getRandomIntLessThan(2);
	}

	if (axisToMove == 0) // Move on X axis
	{
		if (destination.x > x)
		{
			return IntVector2::XAXIS;
		}
		else
		{
			return -IntVector2::XAXIS;
		}
	}
	else // Move on Y axis
	{
		if (destination.y > y)
		{
			return IntVector2::YAXIS;
		}
		else
		{
			return -IntVector2::YAXIS;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Operators
