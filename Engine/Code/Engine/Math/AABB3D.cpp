#include "Engine/Math/AABB3D.hpp"
//////////////////////////////////////////////////////////////////////////
const AABB3D AABB3D::ZERO_TO_ONE(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
const AABB3D AABB3D::NEG_ONE_TO_ONE(-1.f, -1.f, -1.f, 1.f, 1.f, 1.f);
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Accessors
bool AABB3D::isPointInside(const Vector3& point) const
{
	if (point.x <= mins.x)
		return false;
	if (point.x >= maxs.x)
		return false;
	if (point.y <= mins.y)
		return false;
	if (point.y >= maxs.y)
		return false;
	if (point.z <= mins.z)
		return false;

	return true;
}

bool AABB3D::isPointTouching(const Vector3& point) const
{
	if (point.x < mins.x)
		return false;
	if (point.x > maxs.x)
		return false;
	if (point.y < mins.y)
		return false;
	if (point.y > maxs.y)
		return false;
	if (point.z < mins.z)
		return false;

	return true;
}

const Vector3 AABB3D::calcDimensions() const
{
	float xSize = maxs.x - mins.x;
	float ySize = maxs.y - mins.y;
	float zSize = maxs.z - mins.z;
	return Vector3(xSize, ySize, zSize);
}

const Vector3 AABB3D::calcCenter() const
{
	float xPos = (maxs.x + mins.x) * 0.5f;
	float yPos = (maxs.y + mins.y) * 0.5f;
	float zPos = (maxs.z + mins.z) * 0.5f;
	return Vector3(xPos, yPos, zPos);
}

//////////////////////////////////////////////////////////////////////////
// Mutators
void AABB3D::stretchToIncludePoint(const Vector3& point)
{
	if (point.x < mins.x)
		mins.x = point.x;
	else if (point.x > maxs.x)
		maxs.x = point.x;
	if (point.y < mins.y)
		mins.y = point.y;
	else if (point.y > maxs.y)
		maxs.y = point.y;
	if(point.z > maxs.z)
		maxs.z = point.z;
	else if(point.z < mins.z)
		mins.z = point.z;
}

void AABB3D::addPaddingToSides(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius)
{
	mins.x -= xPaddingRadius;
	mins.y -= yPaddingRadius;
	mins.z -= zPaddingRadius;
	maxs.x += xPaddingRadius;
	maxs.y += yPaddingRadius;
	maxs.z += zPaddingRadius;
}

void AABB3D::translate(const Vector3& translation)
{
	mins += translation;
	maxs += translation;
}

//////////////////////////////////////////////////////////////////////////
// Operators
const AABB3D AABB3D::operator+(const Vector3& translation) const
{
	return AABB3D(mins + translation, maxs + translation);
}

const AABB3D AABB3D::operator-(const Vector3& antiTranslation) const
{
	return AABB3D(mins - antiTranslation, maxs - antiTranslation);
}

const AABB3D interpolate(const AABB3D& start, const AABB3D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	AABB3D blended;
	blended.maxs = (fractionOfStart * start.maxs) + (fractionToEnd * end.maxs);
	blended.mins = (fractionOfStart * start.mins) + (fractionToEnd * end.mins);
	return blended;
}

void AABB3D::operator+=(const Vector3& translation)
{
	mins += translation;
	maxs += translation;
}

void AABB3D::operator-=(const Vector3& antiTranslation)
{
	mins -= antiTranslation;
	maxs -= antiTranslation;
}

bool doAABB3DsOverlap(const AABB3D& a, const AABB3D& b)
{
	if (a.maxs.x <= b.mins.x)
		return false;
	if (b.maxs.x <= a.mins.x)
		return false;
	if (a.maxs.y <= b.mins.y)
		return false;
	if (b.maxs.y <= a.mins.y)
		return false;
	if (a.maxs.z <= b.mins.z)
		return false;
	if (b.maxs.z <= a.mins.z)
		return false;

	return true;
}
