#include "Game/PursueBehavior.hpp"

#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Game/Controller.hpp"
#include "Game/Pawn.hpp"
#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
PursueBehavior::PursueBehavior(const XMLNode& /*element*/)
	:Behavior()
{
	m_name = "Pursue";
}

PursueBehavior::PursueBehavior(const PursueBehavior& /*copy*/)
	:Behavior()
{
	m_name = "Pursue";
}
//////////////////////////////////////////////////////////////////////////
void PursueBehavior::DebugRender() const
{
	BitmapFont* f = g_renderer->CreateOrGetBitmapFont(g_bitmapFont);
	g_renderer->DrawText2D(Vector2(0.f, 0.f), Stringf("Behavior: %s", m_name.c_str()), 20.f, Rgba::WHITE, 1.f, f);
	if (!m_currentPath.empty())
	{
		IntVector2 coords = m_currentPath.back()->m_tileCoords;
		g_renderer->DrawText2D(Vector2(20.f, -20.f), Stringf("Next Tile: %i, %i", coords.x, coords.y), 20.f, Rgba::WHITE, 1.f, f);
		coords = m_currentPath[0]->m_tileCoords;
		g_renderer->DrawText2D(Vector2(20.f, -40.f), Stringf("Final Tile: %i, %i", coords.x, coords.y), 20.f, Rgba::WHITE, 1.f, f);

		g_renderer->translate2D(Vector2(0.f, -60.f));
	}
	else
		g_renderer->translate2D(Vector2(0.f, -20.f));
}

void PursueBehavior::Act(Pawn& pawn)
{
	Pursue_AStarToTarget(pawn);

	++m_turnsInBehavior;
}

Behavior* PursueBehavior::Clone() const
{
	return new PursueBehavior(*this);
}

float PursueBehavior::CalcUtility(Pawn& pawn) const
{
	//Placing arbitrary numbers to get things working will fix to something better and more dynamic later
	if (!pawn.mo_target)
		return 0.f;

	else
		return .8f;
}

void PursueBehavior::Pursue_RandomDirectionTowardFirstPlayer(Pawn& pawn)
{
	// This is currently assuming that their is only one pawn the player will ever control
	const Pawn& playerPawn = *Controller::s_controllers[0]->mo_controlledPawn;

	IntVector2 pawnsPosition = pawn.getTileCoords();

	// Move in a random direction towards the player
	int axisToMove = getRandomIntLessThan(2);
	if (axisToMove == 0) // Move on X axis
	{
		if (playerPawn.getTileCoords().x > pawnsPosition.x)
		{
			pawn.MoveToTile(pawnsPosition + IntVector2(1, 0));
		}
		else
		{
			pawn.MoveToTile(pawnsPosition + IntVector2(-1, 0));
		}
	}
	else // Move on Y axis
	{
		if (playerPawn.getTileCoords().y > pawnsPosition.y)
		{
			pawn.MoveToTile(pawnsPosition + IntVector2(0, 1));
		}
		else
		{
			pawn.MoveToTile(pawnsPosition + IntVector2(0, -1));
		}
	}
}

void PursueBehavior::Pursue_AStarToTarget(Pawn& pawn)
{
	pawn.mo_currentMap->GeneratePathForActor(pawn.getTileCoords(), pawn.mo_target->getTileCoords(), m_currentPath, &pawn);

	if(m_currentPath.back() == pawn.mo_currentTile)
		m_currentPath.pop_back();

	if (pawn.MoveToTile(m_currentPath.back()))
		m_currentPath.pop_back();
}
