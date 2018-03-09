#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


const AABB2 AABB2::ZERO_TO_ONE(0.f, 0.f, 1.f, 1.f);
const AABB2 AABB2::NEG_ONE_TO_ONE(-1.f, -1.f, 1.f, 1.f);

//////////////////////////////////////////////////////////////////////////
// Constructors

//////////////////////////////////////////////////////////////////////////
// Accessors
bool AABB2::isPointInside(const Vector2& point) const
{
	if (point.x <= mins.x)
		return false;
	if (point.x >= maxs.x)
		return false;
	if (point.y <= mins.y)
		return false;
	if (point.y >= maxs.y)
		return false;

	return true;
}

bool AABB2::isPointTouching(const Vector2& point) const
{
	if (point.x < mins.x)
		return false;
	if (point.x > maxs.x)
		return false;
	if (point.y < mins.y)
		return false;
	if (point.y > maxs.y)
		return false;

	return true;
}

const Vector2 AABB2::calcDimensions() const
{
	float xSize = maxs.x - mins.x;
	float ySize = maxs.y - mins.y;
	return Vector2(xSize, ySize);
}

const Vector2 AABB2::calcCenter() const
{
	float xPos = (maxs.x + mins.x) * 0.5f;
	float yPos = (maxs.y + mins.y) * 0.5f;
	return Vector2(xPos, yPos);
}

const Vector2 AABB2::findClosetPoint(const Vector2& point) const
{
	float nearestX = clamp(point.x, mins.x, maxs.x);
	float nearestY = clamp(point.y, mins.y, maxs.y);
	return Vector2(nearestX, nearestY);
}

const Vector2 AABB2::getPoint(const Vector2& uv) const
{
	return rangeMap(uv, Vector2::ZERO, Vector2::ONE, mins, maxs);
}

const Vector2 AABB2::getNormalizedPoint(const Vector2& uv) const
{
	return rangeMap(uv, mins, maxs, Vector2::ZERO, Vector2::ONE);
}
//////////////////////////////////////////////////////////////////////////
void AABB2::stretchToIncludePoint(const Vector2& point)
{
	if (point.x < mins.x)
		mins.x = point.x;
	else if (point.x > maxs.x)
		maxs.x = point.x;
	if (point.y < mins.y)
		mins.y = point.y;
	else if (point.y > maxs.y)
		maxs.y = point.y;
}

void AABB2::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius)
{
	mins.x -= xPaddingRadius;
	mins.y -= yPaddingRadius;
	maxs.x += xPaddingRadius;
	maxs.y += yPaddingRadius;
}

void AABB2::translate(const Vector2& translation)
{
	mins += translation;
	maxs += translation;
}

//////////////////////////////////////////////////////////////////////////
// Operators
const AABB2 AABB2::operator+(const Vector2& translation) const
{
	return AABB2(mins + translation, maxs + translation);
}

const AABB2 AABB2::operator-(const Vector2& antiTranslation) const
{
	return AABB2(mins - antiTranslation, maxs - antiTranslation);
}

void AABB2::operator+=(const Vector2& translation)
{
	mins += translation;
	maxs += translation;
}

void AABB2::operator-=(const Vector2& antiTranslation)
{
	mins -= antiTranslation;
	maxs -= antiTranslation;
}

const AABB2 interpolate(const AABB2& start, const AABB2& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	AABB2 blended;
	blended.maxs = (fractionOfStart * start.maxs) + (fractionToEnd * end.maxs);
	blended.mins = (fractionOfStart * start.mins) + (fractionToEnd * end.mins);
	return blended;
}



bool doAABBsOverlap(const AABB2& a, const AABB2& b)
{
	if (a.maxs.x <= b.mins.x)
		return false;
	if (b.maxs.x <= a.mins.x)
		return false;
	if (a.maxs.y <= b.mins.y)
		return false;
	if (b.maxs.y <= a.mins.y)
		return false;
	
	return true;
}
