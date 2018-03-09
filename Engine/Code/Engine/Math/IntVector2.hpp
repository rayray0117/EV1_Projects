#pragma once
#include "Engine/Math/Vector2.hpp"
//////////////////////////////////////////////////////////////////////////
class IntVector2
{
public:
	int x;
	int y;
	
	static const IntVector2 ZERO;
	static const IntVector2 ONE;
	static const IntVector2 XAXIS;
	static const IntVector2 YAXIS;
public:
	IntVector2();
	IntVector2(const IntVector2& copy);
	explicit IntVector2(int xPos, int yPos);
	explicit IntVector2(const Vector2& copy);
	~IntVector2();

	// getAsArray
	const int* getAsIntArray() const;
	int* getAsIntArray();

	//Accessors
	void getXY(int& outX, int& outY) const;

	// Mutators
	void setXY(int newX, int newY);


	// Operators
	bool operator==(const IntVector2& rightHandSide) const;
	bool operator!=(const IntVector2& rightHandSide) const;
	const IntVector2 operator+(const IntVector2& rightHandSide) const;
	const IntVector2 operator-(const IntVector2& rightHandSide) const;
	const IntVector2 operator-() const;
	const IntVector2 operator*(int scale) const;
	const IntVector2 operator*(const IntVector2& perAxisScaleFactors) const;
	void operator*=(int scale);
	void operator*=(const IntVector2& perAxisScaleFactors);
	void operator+=(const IntVector2& rightHandSide);
	void operator-=(const IntVector2& rightHandSide);
#ifdef INTVEC2_MORE_COMPARES
	bool operator>=(const IntVector2& rightHandSide) const; //If both values are >= rightHandSide it returns true, otherwise false
	bool operator<(const IntVector2& rightHandSide) const; //If both values are < rightHandSide it returns true, otherwise false
	bool operator<=(const IntVector2& rightHandSide) const; //If both values are < rightHandSide it returns true, otherwise false
#endif
	const IntVector2 getRandomDirectionToward(const IntVector2& destination) const;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inlines
//////////////////////////////////////////////////////////////////////////
// Contrustors
inline IntVector2::IntVector2()
{
}

inline IntVector2::IntVector2(const IntVector2& copy)
	: x(copy.x)
	, y(copy.y)
{
}

inline IntVector2::IntVector2(int xPos, int yPos)
	: x(xPos)
	, y(yPos)
{
}

inline IntVector2::~IntVector2()
{
}
//////////////////////////////////////////////////////////////////////////
// Operators
inline const IntVector2 IntVector2::operator-(const IntVector2& rightHandSide) const
{
	return IntVector2(x - rightHandSide.x, y - rightHandSide.y);
}

inline const IntVector2 IntVector2::operator-() const
{
	return IntVector2(-x, -y);
}

inline const IntVector2 IntVector2::operator*(int scale) const
{
	return IntVector2(x * scale, y * scale);
}

inline const IntVector2 IntVector2::operator*(const IntVector2& perAxisScaleFactors) const
{
	return IntVector2(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y);
}

inline void IntVector2::operator-=(const IntVector2& rightHandSide)
{
	x -= rightHandSide.x;
	y -= rightHandSide.y;
}

inline void IntVector2::operator+=(const IntVector2& rightHandSide)
{
	x += rightHandSide.x;
	y += rightHandSide.y;
}

inline void IntVector2::operator*=(const IntVector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

inline void IntVector2::operator*=(int scale)
{
	x *= scale;
	y *= scale;
}

inline bool IntVector2::operator!=(const IntVector2& rightHandSide) const
{
	return ((x != rightHandSide.x) || (y != rightHandSide.y)); // equivalent to !((x == rightHandSide.x) && (y == rightHandSide.y));
}

inline const IntVector2 IntVector2::operator+(const IntVector2& rightHandSide) const
{
	return IntVector2(x + rightHandSide.x, y + rightHandSide.y);
}

inline bool IntVector2::operator==(const IntVector2& rightHandSide) const
{
	return rightHandSide.x == x && rightHandSide.y == y;
}
#ifdef INTVEC2_MORE_COMPARES
inline bool IntVector2::operator>=(const IntVector2& rightHandSide) const
{
	return (x >= rightHandSide.x) && (y >= rightHandSide.y);
}

inline bool IntVector2::operator<(const IntVector2& rightHandSide) const
{
	return (x < rightHandSide.x) && (y < rightHandSide.y);
}

inline bool IntVector2::operator<=(const IntVector2& rightHandSide) const
{
	return (x <= rightHandSide.x) && (y <= rightHandSide.y);
}
#endif
