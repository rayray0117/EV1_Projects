#pragma once
#include "Engine/Math/Vector2.hpp"

class Disc2D
{
public:
	Vector2 center;
	float radius;

	static const Disc2D UNIT_CIRCLE;
public:
	Disc2D();
	Disc2D(const Disc2D& copy);
	explicit Disc2D(float initialX, float initialY, float initialRadius);
	explicit Disc2D(const Vector2& initialCenter, float initialRadius);
	~Disc2D();

	// Accessors
	bool isPointInside(const Vector2& point) const;
	bool isPointTouching(const Vector2& point) const;

	// Mutators
	void stretchToIncludePoint(const Vector2& point);
	void addPadding(float paddingRadius);
	void translate(const Vector2& translation);

	// Operators
	const Disc2D operator+(const Vector2& translation) const;
	const Disc2D operator-(const Vector2& antiTranslation) const;
	void operator+=(const Vector2& translation);
	void operator-=(const Vector2& antiTranslation);

	friend const Disc2D Interpolate(const Disc2D& start, const Disc2D& end, float fractionToEnd);
};

bool DoDiscsOverlap(const Disc2D& a, const Disc2D& b);
bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius);

void DiscVsDiscCollision(Disc2D& a, Vector2& aVelocity, Disc2D& b, Vector2& bVelocity, float aElasticity = 1.0f, float bElasticy = 1.0f);
void DiscVsStationaryDiscCollision(Disc2D& movingDisc, Vector2& aVelocity, const Disc2D& nonMovingDisc, float aElasticity = 1.0f, float nonMovingElasticy = 1.0f);
void DiscVsStationaryDiscOverlapCorrection(Disc2D& movingDisc, const Disc2D& nonMovingDisc);
void DiscVsDiscOverlapCorrection(Disc2D& a, Disc2D& b);