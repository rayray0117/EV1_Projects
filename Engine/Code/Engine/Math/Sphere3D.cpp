#include "Engine/Math/Sphere3D.hpp"

Sphere3D::Sphere3D()
{
}

Sphere3D::Sphere3D(const Sphere3D& copy)
	: center(copy.center)
	, radius(copy.radius)
{
}

Sphere3D::Sphere3D(float initialX, float initialY, float initialZ, float initialRadius)
	: center(initialX, initialY, initialZ)
	, radius(initialRadius)
{
}

Sphere3D::Sphere3D(const Vector3& initialCenter, float initialRadius)
	: center(initialCenter)
	, radius(initialRadius)
{
}

Sphere3D::~Sphere3D()
{
}

//////////////////////////////////////////////////////////////////////////
// Accessors
bool Sphere3D::isPointInside(const Vector3& point) const
{
	return calcDistanceSquared(center, point) < (radius * radius);
}

bool Sphere3D::isPointTouching(const Vector3& point) const
{
	return (calcDistanceSquared(center, point) <= (radius * radius));
}

//////////////////////////////////////////////////////////////////////////
// Mutators
void Sphere3D::stretchToIncludePoint(const Vector3& point)
{
	if (!isPointTouching(point))
	{
		radius = calcDistance(center, point);
	}
}

void Sphere3D::addPadding(float paddingRadius)
{
	radius += paddingRadius;
}

void Sphere3D::translate(const Vector3& translation)
{
	center += translation;
}

//////////////////////////////////////////////////////////////////////////
// Operators
const Sphere3D Sphere3D::operator+(const Vector3& translation) const
{
	return Sphere3D(center + translation, radius);
}

const Sphere3D Sphere3D::operator-(const Vector3& antiTranslation) const
{
	return Sphere3D(center - antiTranslation, radius);
}

void Sphere3D::operator+=(const Vector3& translation)
{
	center += translation;
}

void Sphere3D::operator-=(const Vector3& antiTranslation)
{
	center -= antiTranslation;
}

const Sphere3D interpolate(const Sphere3D& start, const Sphere3D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	Sphere3D blended;
	blended.center = (fractionOfStart * start.center) + (fractionToEnd * end.center);
	blended.radius = (fractionOfStart * start.radius) + (fractionToEnd * end.radius);
	return blended;
}


//////////////////////////////////////////////////////////////////////////
bool doSpheresOverlap(const Sphere3D& a, const Sphere3D& b)
{
	float distanceSquared = calcDistanceSquared(a.center, b.center);
	float sumOfRadii = (a.radius + b.radius);
	return (distanceSquared < (sumOfRadii * sumOfRadii));
}
