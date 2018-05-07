#include "Game/AttackBehavior.hpp"

#include "Game/Pawn.hpp"
#include "Game/Map.hpp"
//////////////////////////////////////////////////////////////////////////

AttackBehavior::AttackBehavior(const XMLNode& /*element*/)
{
	m_name = "Attack";
}

AttackBehavior::AttackBehavior(const AttackBehavior& /*copy*/)
{
	m_name = "Attack";
}
//////////////////////////////////////////////////////////////////////////
void AttackBehavior::Act(Pawn& pawn)
{
	pawn.AttackTile(pawn.mo_target->mo_currentTile);
}

Behavior* AttackBehavior::Clone() const
{
	return new AttackBehavior(*this);
}

float AttackBehavior::CalcUtility(Pawn& pawn) const
{
	TODO("Fix this to be a call on the Pawn that check if they are in range rather than crude hard coding!")
	if (pawn.mo_target && pawn.mo_currentMap->CalcManhattanDist(pawn.getTileCoords(), pawn.mo_target->getTileCoords()) <= 1)
		return .86f;

	return 0.f;
}
