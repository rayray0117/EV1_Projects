#include "Game/WanderBehavior.hpp"

#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Game/Controller.hpp"
#include "Game/Pawn.hpp"
#include "Game/Tile.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
WanderBehavior::WanderBehavior(const XMLNode& /*element*/)
	:Behavior()
{
	m_name = "Wander";
}

WanderBehavior::WanderBehavior(const WanderBehavior& /*copy*/)
	:Behavior()
{
	m_name = "Wander";
}
//////////////////////////////////////////////////////////////////////////
void WanderBehavior::DebugRender() const
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

void WanderBehavior::Act(Pawn& pawn)
{
	Wander_RandomMapPosPath(pawn);

	++m_turnsInBehavior;
}

Behavior* WanderBehavior::Clone() const
{
	return new WanderBehavior(*this);
}

float WanderBehavior::CalcUtility(Pawn& pawn) const
{
	//Placing arbitrary numbers to get things working will fix to something better and more dynamic later

	if (pawn.mo_target)
		return .1f;

	return .7f;
}

void WanderBehavior::Wander_RandomDirection(Pawn& pawn)
{
	IntVector2 pawnsPosition = pawn.getTileCoords();

	// Move in a random direction
	int direction = getRandomIntLessThan(4);
	switch (direction)
	{
	case 0: pawn.MoveToTile(pawnsPosition + IntVector2(-1, 0));
		break;
	case 1: pawn.MoveToTile(pawnsPosition + IntVector2(1, 0));
		break;
	case 2:	pawn.MoveToTile(pawnsPosition + IntVector2(0, -1));
		break;
	case 3:	pawn.MoveToTile(pawnsPosition + IntVector2(0, 1));
		break;
	default:
		break;
	}
}

void WanderBehavior::Wander_RandomMapPosPath(Pawn& pawn)
{
	if (m_currentPath.empty() || m_turnsInBehavior > (m_estTurnsNeededToReachGoal + m_maxNumberOfWastedTurns))
	{
		m_turnsInBehavior = 0; //Set it back to 0 so I know how long I've been trying to get to this destination.

		PickNewRandomMapPosPath(pawn);
	}


	if (m_currentPath.back() == pawn.mo_currentTile)
		m_currentPath.pop_back();
	//If I was successful in getting here pop_back
	if(pawn.MoveToTile(m_currentPath.back()))
		m_currentPath.pop_back();
}

void WanderBehavior::PickNewRandomMapPosPath(Pawn& pawn)
{
	//Pick a random not solid tile.
	const Tile* goalTile = pawn.mo_currentMap->getRandomTile();
	while (goalTile->isSolid())
	{
		goalTile = pawn.mo_currentMap->getRandomTile();
	}

	IntVector2 pawnsPosition = pawn.getTileCoords();
	pawn.mo_currentMap->GeneratePathForActor(pawnsPosition, goalTile->m_tileCoords, m_currentPath, &pawn);
	m_estTurnsNeededToReachGoal = m_currentPath.size();
}
