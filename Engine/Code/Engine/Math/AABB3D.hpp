#pragma once
#include "Engine/Math/Vector3.hpp"

class AABB3D
{
public:
	Vector3 mins;
	Vector3 maxs;

	static const AABB3D ZERO_TO_ONE;
	static const AABB3D NEG_ONE_TO_ONE;
public:
	AABB3D();
	AABB3D(const AABB3D& copy);
	explicit AABB3D(float initialX, float initialY, float initialZ);
	explicit AABB3D(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	explicit AABB3D(const Vector3& mins, const Vector3& maxs);
	explicit AABB3D(const Vector3& center, float radiusX, float radiusY, float radiusZ);
	~AABB3D();

	// Accessors
	bool isPointInside(const Vector3& point) const;
	bool isPointTouching(const Vector3& point) const;
	const Vector3 calcDimensions() const;
	const Vector3 calcCenter() const;

	// Mutators
	void stretchToIncludePoint(const Vector3& point);
	void addPaddingToSides(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius);
	void translate(const Vector3& translation);

	// Operators
	const AABB3D operator+(const Vector3& translation) const;
	const AABB3D operator-(const Vector3& antiTranslation) const;
	void operator+=(const Vector3& translation);
	void operator-=(const Vector3& antiTranslation);

	friend const AABB3D interpolate(const AABB3D& start, const AABB3D& end, float fractionToEnd);
};

bool doAABB3DsOverlap(const AABB3D& a, const AABB3D& b);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inlines
//////////////////////////////////////////////////////////////////////////
// Constructors
inline AABB3D::AABB3D()
{
}

inline AABB3D::AABB3D(const AABB3D& copy)
	: maxs(copy.maxs)
	, mins(copy.mins)
{
}

inline AABB3D::AABB3D(float initialX, float initialY, float initialZ)
	: mins(initialX, initialY, initialZ)
	, maxs(initialX, initialY, initialZ)
{
}

inline AABB3D::AABB3D(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	: mins(minX, minY, minZ)
	, maxs(maxX, maxY, maxZ)
{
}

inline AABB3D::AABB3D(const Vector3& mins, const Vector3& maxs)
	: mins(mins)
	, maxs(maxs)
{
}

inline AABB3D::AABB3D(const Vector3& center, float radiusX, float radiusY, float radiusZ)
	: mins(center.x - radiusX, center.y - radiusY, center.z - radiusZ)
	, maxs(center.x + radiusX, center.y + radiusY, center.z + radiusZ)
{
}

inline AABB3D::~AABB3D()
{
}