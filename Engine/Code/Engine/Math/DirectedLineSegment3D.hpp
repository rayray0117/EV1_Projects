#pragma once
#include "Engine/Math/Vector3.hpp"

class DirectedLineSegment3D
{
public:
	Vector3 startPos;
	Vector3 endPos;

public:
	DirectedLineSegment3D();
	explicit DirectedLineSegment3D(float startX, float startY, float startZ, float endX, float endY, float endZ);
	explicit DirectedLineSegment3D(Vector3 start, Vector3 end);
	~DirectedLineSegment3D();

	friend const DirectedLineSegment3D interpolate(const DirectedLineSegment3D& start, const DirectedLineSegment3D& end, float fractionToEnd);
};