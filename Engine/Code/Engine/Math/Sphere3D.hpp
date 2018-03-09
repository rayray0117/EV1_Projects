#pragma once
#include "Engine/Math/Vector3.hpp"

class Sphere3D
{
public:
	Vector3 center;
	float radius;

	static const Sphere3D UNIT_SPHERE;
public:
	Sphere3D();
	Sphere3D(const Sphere3D& copy);
	explicit Sphere3D(float initialX, float initialY, float initialZ, float initialRadius);
	explicit Sphere3D(const Vector3& initialCenter, float initialRadius);
	~Sphere3D();

	// Accessors
	bool isPointInside(const Vector3& point) const;
	bool isPointTouching(const Vector3& point) const;

	// Mutators
	void stretchToIncludePoint(const Vector3& point);
	void addPadding(float paddingRadius);
	void translate(const Vector3& translation);

	// Operators
	const Sphere3D operator+(const Vector3& translation) const;
	const Sphere3D operator-(const Vector3& antiTranslation) const;
	void operator+=(const Vector3& translation);
	void operator-=(const Vector3& antiTranslation);

	friend const Sphere3D interpolate(const Sphere3D& start, const Sphere3D& end, float fractionToEnd);
};

bool doSpheresOverlap(const Sphere3D& a, const Sphere3D& b);