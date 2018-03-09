#include "Engine/Math/DirectedLineSegment2D.hpp"

DirectedLineSegment2D::DirectedLineSegment2D()
	: startPos(0.f, 0.f)
	, endPos(0.f,0.f)
{
}

DirectedLineSegment2D::DirectedLineSegment2D(float startX, float startY, float endX, float endY)
	: startPos(startX, startY)
	, endPos(endX, endY)
{
}

DirectedLineSegment2D::DirectedLineSegment2D(Vector2 start, Vector2 end)
	: startPos(start)
	, endPos(end)
{
}

DirectedLineSegment2D::~DirectedLineSegment2D()
{
}

const DirectedLineSegment2D interpolate(const DirectedLineSegment2D& start, const DirectedLineSegment2D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	DirectedLineSegment2D blended;
	blended.endPos = (fractionOfStart * start.endPos) + (fractionToEnd * end.endPos);
	blended.startPos = (fractionOfStart * start.startPos) + (fractionToEnd * end.startPos);
	return blended;
}
