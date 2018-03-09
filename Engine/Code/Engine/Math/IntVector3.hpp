#pragma once
#include "Engine/Math/Vector3.hpp"

class IntVector3
{
public:
	int x;
	int y;
	int z;

	static const IntVector3 ZERO;
	static const IntVector3 ONE;
	static const IntVector3 XAXIS;
	static const IntVector3 YAXIS;
	static const IntVector3 ZAXIS;
public:
	IntVector3();
	IntVector3(const IntVector3& copy);
	explicit IntVector3(int xPos, int yPos, int zPos);
	IntVector3(Vector3 copy);
	~IntVector3();

	const int* getAsIntArray() const;
	int* getAsIntArray();

	//Accessors
	void getXY(int& outX, int& outY, int& outZ) const;

	// Mutators
	void setXY(int newX, int newY, int newZ);

	bool operator==(const IntVector3& rightHandSide) const;
	bool operator!=(const IntVector3& rightHandSide) const;
	const IntVector3 operator+(const IntVector3& rightHandSide) const;
	const IntVector3 operator-(const IntVector3& rightHandSide) const;
	const IntVector3 operator-() const;
	const IntVector3 operator*(int scale) const;
	const IntVector3 operator*(const IntVector3& perAxisScaleFactors) const;
	void operator*=(int scale);
	void operator*=(const IntVector3& perAxisScaleFactors);
	void operator+=(const IntVector3& rightHandSide);
	void operator-=(const IntVector3& rightHandSide);
	const IntVector3 operator/(float inverseScale);
};

//////////////////////////////////////////////////////////////////////////
// Inline
//////////////////////////////////////////////////////////////////////////
// Constructors
inline IntVector3::IntVector3()
{
}

inline IntVector3::IntVector3(const IntVector3& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
{
}

inline IntVector3::IntVector3(int xPos, int yPos, int zPos)
	: x(xPos)
	, y(yPos)
	, z(zPos)
{
}

inline IntVector3::~IntVector3()
{
}
//////////////////////////////////////////////////////////////////////////
// Operators
inline const IntVector3 IntVector3::operator-(const IntVector3& rightHandSide) const
{
	return IntVector3(x - rightHandSide.x, y - rightHandSide.y, z - rightHandSide.z);
}

inline const IntVector3 IntVector3::operator-() const
{
	return IntVector3(-x, -y, -z);
}

inline const IntVector3 IntVector3::operator*(int scale) const
{
	return IntVector3(x * scale, y * scale, z * scale);
}

inline const IntVector3 IntVector3::operator*(const IntVector3& perAxisScaleFactors) const
{
	return IntVector3(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z);
}

inline void IntVector3::operator-=(const IntVector3& rightHandSide)
{
	x -= rightHandSide.x;
	y -= rightHandSide.y;
	z -= rightHandSide.z;
}

inline void IntVector3::operator+=(const IntVector3& rightHandSide)
{
	x += rightHandSide.x;
	y += rightHandSide.y;
	z += rightHandSide.z;
}

inline void IntVector3::operator*=(const IntVector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

inline void IntVector3::operator*=(int scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

inline bool IntVector3::operator!=(const IntVector3& rightHandSide) const
{
	return ((x != rightHandSide.x) || (y != rightHandSide.y) || (z != rightHandSide.z)); // equivalent to !((x == rightHandSide.x) && (y == rightHandSide.y));
}

inline const IntVector3 IntVector3::operator+(const IntVector3& rightHandSide) const
{
	return IntVector3(x + rightHandSide.x, y + rightHandSide.y, z + rightHandSide.z);
}

inline bool IntVector3::operator==(const IntVector3& rightHandSide) const
{
	return rightHandSide.x == x && rightHandSide.y == y && rightHandSide.z == z;
}