#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Matrix4.hpp"
//////////////////////////////////////////////////////////////////////////
struct SQT
{
public:
	Vector3 scale;
	Quaternion rotation;
	Vector3 position;
public:
	Matrix4 getAsMatrix() const;
	void Combine(const SQT& transform);
	SQT getCombined(const SQT& transform) const;
	void setFromMatrix(const Matrix4& inMatrix);

	friend const SQT interpolate(const SQT& start, const SQT& end, float fractionToEnd);
	SQT getInverse() const;
};