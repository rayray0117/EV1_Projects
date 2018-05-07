#include "Game/FleeBehavior.hpp"

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
FleeBehavior::FleeBehavior(const XMLNode& /*element*/)
	:Behavior()
{
	m_name = "Flee";
}

FleeBehavior::FleeBehavior(const FleeBehavior& /*copy*/)
	: Behavior()
{
	m_name = "Flee";
}
//////////////////////////////////////////////////////////////////////////
void FleeBehavior::DebugRender() const
{
	BitmapFont* f = g_renderer->CreateOrGetBitmapFont(g_bitmapFont);
	g_renderer->DrawText2D(Vector2::ZERO, Stringf("Behavior: %s", m_name.c_str()), 20.f, Rgba::WHITE, 1.f, f);
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

void FleeBehavior::Act(Pawn& pawn)
{
	Flee_RandomPathAway(pawn);

	++m_turnsInBehavior;
}

Behavior* FleeBehavior::Clone() const
{
	return new FleeBehavior(*this);
}

float FleeBehavior::CalcUtility(Pawn& pawn) const
{
	if (pawn.getCurrentHealthPercent() <= .15 && pawn.mo_target)
		return 1.f;

	return 0.f;
}

void FleeBehavior::Flee_RandomDirectionAway(Pawn& pawn)
{
	// This is currently assuming that their is only one pawn the player will ever control
	const Pawn& playerPawn = *Controller::s_controllers[0]->mo_controlledPawn;

	IntVector2 pawnsPosition = pawn.getTileCoords();

	// Move in a random direction away from the player
	int axisToMove = getRandomIntLessThan(2);
	if (axisToMove == 0) // Move on X axis
	{
		if (playerPawn.getTileCoords().x > pawnsPosition.x)
		{
			pawn.MoveToTile(pawnsPosition + IntVector2(-1, 0));
		}
		else
		{
			pawn.MoveToTile(pawnsPosition + IntVector2(1, 0));
		}
	}
	else // Move on Y axis
	{
		if (playerPawn.getTileCoords().y > pawnsPosition.y)
		{
			pawn.MoveToTile(pawnsPosition + IntVector2(0, -1));
		}
		else
		{
			pawn.MoveToTile(pawnsPosition + IntVector2(0, 1));
		}
	}
}

void FleeBehavior::Flee_RandomPathAway(Pawn& pawn)
{
	if (m_currentPath.empty() /*|| m_turnsInBehavior > (m_estTurnsNeededToReachGoal + m_maxNumberOfWastedTurns)*/)
	{
		const int numPlaces = 10;
		IntVector2 farestPlaceToRun;
		int farestDist = 0;
		for (int i = 0; i < numPlaces; i++)
		{
			const Tile* temp = pawn.mo_currentMap->getRandomTile();
			while (temp->isSolid()) { temp = pawn.mo_currentMap->getRandomTile(); }

			IntVector2 possiblePlaceToRun = temp->m_tileCoords;
			int dist = pawn.mo_currentMap->CalcManhattanDist(possiblePlaceToRun, pawn.getTileCoords());

			if (dist > farestDist)
			{
				farestPlaceToRun = possiblePlaceToRun;
			}
		}

		pawn.mo_currentMap->GeneratePathForActor(pawn.getTileCoords(), farestPlaceToRun, m_currentPath, &pawn);
	}

	if (m_currentPath.back() == pawn.mo_currentTile)
		m_currentPath.pop_back();
	if (pawn.MoveToTile(m_currentPath.back()))
		m_currentPath.pop_back();
}
