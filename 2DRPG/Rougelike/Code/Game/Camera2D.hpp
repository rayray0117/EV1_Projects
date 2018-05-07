#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/IntVector2.hpp"
//////////////////////////////////////////////////////////////////////////
class Camera2D
{
public:
	Camera2D();
	~Camera2D() {}
	
	void Update(float deltaSeconds);
	void Render() const;

	void SetPosition(const IntVector2& centerPosition){m_centerPosition = centerPosition;}
	void SetViewRange(uint range) { m_viewRange.x = range; m_viewRange.y = range; }
	void ToggleViewEntireMap() { m_viewEntireMap = !m_viewEntireMap; }
	const void getCameraView(IntVector2& out_bottomLeftTile, IntVector2& out_topRightTile) const;
	const void getOrtho(Vector2& out_bottomLeftTile, Vector2& out_topRightTile) const;
public:
	IntVector2 m_centerPosition;
	IntVector2 m_viewRange;
	bool m_viewEntireMap;
};
