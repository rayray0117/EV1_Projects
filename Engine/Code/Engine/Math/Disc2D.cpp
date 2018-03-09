#include "Engine/Math/Disc2D.hpp"

const Disc2D Disc2D::UNIT_CIRCLE(0.f, 0.f, 1.f);

//////////////////////////////////////////////////////////////////////////
Disc2D::Disc2D()  //#ToDo: find out why this is here again, then remove initializers and make sure all old code will still work.
	: center(0.f, 0.f)
	, radius(0.f)
{
}

Disc2D::Disc2D(const Disc2D& copy)
	: center(copy.center)
	, radius(copy.radius)
{
}

Disc2D::Disc2D(float initialX, float initialY, float initialRadius)
	: center(initialX,initialY)
	, radius(initialRadius)
{
}

Disc2D::Disc2D(const Vector2& initialCenter, float initialRadius)
	: center(initialCenter)
	, radius(initialRadius)
{
}

Disc2D::~Disc2D()
{
}

//////////////////////////////////////////////////////////////////////////
// Accessors
bool Disc2D::isPointInside(const Vector2& point) const
{
	return calcDistanceSquared(center, point) < (radius * radius);
}

bool Disc2D::isPointTouching(const Vector2& point) const
{
	return (calcDistanceSquared(center, point) <= (radius * radius));
}

//////////////////////////////////////////////////////////////////////////
// Mutators
void Disc2D::stretchToIncludePoint(const Vector2& point)
{
	if (!isPointTouching(point))
	{
		radius = calcDistance(center, point);
	}
}

void Disc2D::addPadding(float paddingRadius)
{
	radius += paddingRadius;
}

void Disc2D::translate(const Vector2& translation)
{
	center += translation;
}

//////////////////////////////////////////////////////////////////////////
// Operators
const Disc2D Disc2D::operator+(const Vector2& translation) const
{
	return Disc2D(center + translation, radius);
}

const Disc2D Disc2D::operator-(const Vector2& antiTranslation) const
{
	return Disc2D(center - antiTranslation, radius);
}

void Disc2D::operator+=(const Vector2& translation)
{
	center += translation;
}

void Disc2D::operator-=(const Vector2& antiTranslation)
{
	center -= antiTranslation;
}

const Disc2D Interpolate(const Disc2D& start, const Disc2D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	Disc2D blended;
	blended.center = (fractionOfStart * start.center) + (fractionToEnd * end.center);
	blended.radius = (fractionOfStart * start.radius) + (fractionToEnd * end.radius);
	return blended;

}

bool DoDiscsOverlap(const Disc2D& a, const Disc2D& b)
{
	return DoDiscsOverlap(a.center, a.radius, b.center, b.radius);
}

bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius)
{
	float distanceSquared = calcDistanceSquared(aCenter, bCenter);
	float sumOfRadii = (aRadius + bRadius);
	return (distanceSquared < (sumOfRadii * sumOfRadii));
}

void DiscVsDiscCollision(Disc2D& a, Vector2& aVelocity, Disc2D& b, Vector2& bVelocity, float aElasticity, float bElasticy)
{
	Vector2 displacementAB = b.center - a.center;
	Vector2 directionAB = displacementAB;
	float distanceAB = directionAB.normalize();
	Vector2 displacementBA = a.center - b.center;
	Vector2 directionBA = displacementBA.getNormalizedVector();

	float sumOfRadii = a.radius + b.radius;
	float overlap = sumOfRadii - distanceAB;
	float overlapCorrection = overlap * 0.5f;

	a.center -= directionAB * overlapCorrection;
	b.center -= directionBA * overlapCorrection;

	float speedAlongAB = dotProduct(aVelocity, directionAB);
	float speedAlongBA = dotProduct(bVelocity, directionBA);

	if (speedAlongAB < speedAlongBA)
	{
		return;
	}

	Vector2 velAlongAB = speedAlongAB * directionAB; //velocity of A toward B
	Vector2 velAwayAB = aVelocity - velAlongAB; // velocity of A not toward B
	velAlongAB *= (aElasticity * bElasticy);

	
	Vector2 velAlongBA = speedAlongBA * directionBA;
	Vector2 velAwayBA = bVelocity - velAlongBA;
	velAlongBA *= (aElasticity * bElasticy);

	aVelocity = velAwayAB + velAlongBA;
	bVelocity = velAwayBA + velAlongAB;
}

void DiscVsStationaryDiscCollision(Disc2D& movingDisc, Vector2& aVelocity, const Disc2D& nonMovingDisc, float aElasticity, float nonMovingElasticy)
{
	Vector2 displacementAB = nonMovingDisc.center - movingDisc.center;
	Vector2 directionAB = displacementAB;
	float distanceAB = directionAB.normalize();

	float sumOfRadii = movingDisc.radius + nonMovingDisc.radius;
	float overlap = sumOfRadii - distanceAB;
	
	movingDisc.center -= directionAB * overlap;

	float speedAB = dotProduct(aVelocity, directionAB);
	Vector2 velocityAB = speedAB * directionAB; //velocity of A toward B
	Vector2 velocityAT = aVelocity - velocityAB; // velocity of A not toward B
	velocityAB *= (aElasticity * nonMovingElasticy);
	aVelocity = velocityAT + (velocityAB * -1);
}

void DiscVsStationaryDiscOverlapCorrection(Disc2D& movingDisc, const Disc2D& nonMovingDisc)
{
	Vector2 displacementAB = nonMovingDisc.center - movingDisc.center;
	Vector2 directionAB = displacementAB;
	float distanceAB = directionAB.normalize();

	float sumOfRadii = movingDisc.radius + nonMovingDisc.radius;
	float overlap = sumOfRadii - distanceAB;

	movingDisc.center -= directionAB * overlap;
}

void DiscVsDiscOverlapCorrection(Disc2D& a, Disc2D& b)
{
	Vector2 displacementAB = b.center - a.center;
	Vector2 directionAB = displacementAB;
	float distanceAB = directionAB.normalize();
	Vector2 displacementBA = a.center - b.center;
	Vector2 directionBA = displacementBA.getNormalizedVector();

	float sumOfRadii = a.radius + b.radius;
	float overlap = sumOfRadii - distanceAB;
	float overlapCorrection = overlap * 0.5f;

	a.center -= directionAB * overlapCorrection;
	b.center -= directionBA * overlapCorrection;
}
