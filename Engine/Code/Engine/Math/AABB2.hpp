#pragma once
#include "Engine/Math/Vector2.hpp"

class AABB2
{
public:
	Vector2 mins;
	Vector2 maxs;

	static const AABB2 ZERO_TO_ONE;
	static const AABB2 NEG_ONE_TO_ONE;

public:
	AABB2();
	AABB2(const AABB2& copy);
	explicit AABB2(float initalX, float initialY);
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2(const Vector2& mins, const Vector2& maxs);
	explicit AABB2(const Vector2& center, float radiusX, float radiusY);
	~AABB2();

	// Accessors
	bool isPointInside(const Vector2& point) const;
	bool isPointTouching(const Vector2& point) const;
	const Vector2 calcDimensions() const;
	const Vector2 calcCenter() const;
	const Vector2 findClosetPoint(const Vector2& point) const;
	const Vector2 getPoint(const Vector2& uv) const;
	const Vector2 getNormalizedPoint(const Vector2& uv) const;

	// Mutators
	void stretchToIncludePoint(const Vector2& point);
	void AddPaddingToSides(float xPaddingRadius, float yPaddingRadius);
	void translate(const Vector2& translation);

	// Operators
	const AABB2 operator+(const Vector2& translation) const;
	const AABB2 operator-(const Vector2& antiTranslation) const;
	void operator+=(const Vector2& translation);
	void operator-=(const Vector2& antiTranslation);

	friend const AABB2 interpolate(const AABB2& start, const AABB2& end, float fractionToEnd);
};

bool doAABBsOverlap(const AABB2& a, const AABB2& b);

//////////////////////////////////////////////////////////////////////////
// Inlines
//////////////////////////////////////////////////////////////////////////
// Constructors
inline AABB2::AABB2()
	: maxs(0.f, 0.f)
	, mins(0.f, 0.f)
{
}

inline AABB2::AABB2(const AABB2& copy)
	: maxs(copy.maxs)
	, mins(copy.mins)
{
}

inline AABB2::AABB2(float initalX, float initialY)
	: mins(initalX, initialY)
	, maxs(initalX, initialY)
{
}

inline AABB2::AABB2(float minX, float minY, float maxX, float maxY)
	: mins(minX, minY)
	, maxs(maxX, maxY)
{
}

inline AABB2::AABB2(const Vector2& mins, const Vector2& maxs)
	: mins(mins)
	, maxs(maxs)
{
}

inline AABB2::AABB2(const Vector2& center, float radiusX, float radiusY)
	: mins(center.x - radiusX, center.y - radiusY)
	, maxs(center.x + radiusX, center.y + radiusY)
{
}

inline AABB2::~AABB2()
{

}