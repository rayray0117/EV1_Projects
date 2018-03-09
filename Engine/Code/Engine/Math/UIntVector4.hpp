#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class UIntVector4
{
public:
	uint x;
	uint y;
	uint z;
	uint w;

	static const UIntVector4 ZERO;
	static const UIntVector4 ONE;
	
public:
	UIntVector4() {};
	UIntVector4(const UIntVector4& copy);
	explicit UIntVector4(uint initialX, uint initialY, uint initialZ, uint initialW);

	const uint* getAsArray() const;
	uint* getAsArray();

	// Operators
	bool operator==(const UIntVector4& rightHandSide) const;
	bool operator!=(const UIntVector4& rightHandSide) const;
	const UIntVector4 operator+(const UIntVector4& rightHandSide) const;
	const UIntVector4 operator-(const UIntVector4& rightHandSide) const;
// 	const UIntVector4 operator*(float scale) const;
// 	const UIntVector4 operator*(const UIntVector4& perAxisScaleFactors) const;
// 	const UIntVector4 operator/(float inverseScale);
// 	void operator*=(float scale);
// 	void operator*=(const UIntVector4& perAxisScaleFactors);
	void operator+=(const UIntVector4& rightHandSide);
	void operator-=(const UIntVector4& rightHandSide);
};

//////////////////////////////////////////////////////////////////////////
// Inlines