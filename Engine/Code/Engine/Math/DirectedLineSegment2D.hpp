#pragma once
#include "Engine/Math/Vector2.hpp"


class DirectedLineSegment2D
{
public:
	Vector2 startPos;
	Vector2 endPos;

public:
	DirectedLineSegment2D();
	explicit DirectedLineSegment2D(float startX, float startY, float endX, float endY);
	explicit DirectedLineSegment2D(Vector2 start, Vector2 end);
	~DirectedLineSegment2D();

	friend const DirectedLineSegment2D interpolate(const DirectedLineSegment2D& start, const DirectedLineSegment2D& end, float fractionToEnd);
	// ToDo: Add more when not sick
	// bool doLinesCross
	// bool isPointInside
};