#include "Game/Camera2D.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

#include "Game/World.hpp"
#include "Game/Map.hpp"
//////////////////////////////////////////////////////////////////////////
Camera2D::Camera2D()
	: m_centerPosition(IntVector2::ZERO)
	, m_viewRange(1, 1)
	, m_viewEntireMap(true)
{
}
//////////////////////////////////////////////////////////////////////////
void Camera2D::Update(float /*deltaSeconds*/)
{
	/* DO NOTHING */
}

void Camera2D::Render() const
{
	g_renderer->ClearScreen(Rgba::BLACK);
	if (m_viewEntireMap)
	{
		const IntVector2& topRight = g_theGame->m_theWorld->mo_currentMap->m_dimensions;
		g_renderer->SetOrthoProjection(Vector2::ZERO, topRight);
	}
	else
	{
		g_renderer->SetOrthoProjection(m_centerPosition - m_viewRange, m_centerPosition + m_viewRange + IntVector2::ONE); //Correct for a boxes size5
	}
}

const void Camera2D::getCameraView(IntVector2& out_bottomLeftTile, IntVector2& out_topRightTile) const
{
	out_topRightTile = m_centerPosition + m_viewRange;
	out_bottomLeftTile = m_centerPosition - m_viewRange;
	out_topRightTile += IntVector2::ONE;//Correct for a boxes size
}

const void Camera2D::getOrtho(Vector2& out_bottomLeftTile, Vector2& out_topRightTile) const
{
	if (m_viewEntireMap)
	{
		out_bottomLeftTile = Vector2::ZERO;
		out_topRightTile = g_theGame->m_theWorld->mo_currentMap->m_dimensions;
	}
	else
	{
		out_topRightTile = m_centerPosition + m_viewRange;
		out_bottomLeftTile = m_centerPosition - m_viewRange;
		out_topRightTile += IntVector2::ONE;//Correct for a boxes size
	}
}
