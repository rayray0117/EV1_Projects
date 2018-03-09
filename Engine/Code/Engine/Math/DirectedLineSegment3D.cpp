#include "Engine/Math/DirectedLineSegment3D.hpp"
//////////////////////////////////////////////////////////////////////////
DirectedLineSegment3D::DirectedLineSegment3D()
	: startPos(0.f,0.f,0.f)
	, endPos(0.f, 0.f, 0.f)
{
}

DirectedLineSegment3D::DirectedLineSegment3D(float startX, float startY, float startZ, float endX, float endY, float endZ)
	: startPos(startX, startY, startZ)
	, endPos(endX, endY, endZ)
{
}

DirectedLineSegment3D::DirectedLineSegment3D(Vector3 start, Vector3 end)
	: startPos(start)
	, endPos(end)
{
}

DirectedLineSegment3D::~DirectedLineSegment3D()
{

}

const DirectedLineSegment3D interpolate(const DirectedLineSegment3D& start, const DirectedLineSegment3D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	DirectedLineSegment3D blended;
	blended.endPos = (fractionOfStart * start.endPos) + (fractionToEnd * end.endPos);
	blended.startPos = (fractionOfStart * start.startPos) + (fractionToEnd * end.startPos);
	return blended;
}
