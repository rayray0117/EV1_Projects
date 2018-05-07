#include "Game/Battle.hpp"
#include "Game/Pawn.hpp"
#include "Game/Tile.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
//Temp function to calc simple melee attacks
void Battle::BasicFight(Pawn& attacker, Tile& tileTarget)
{
	Stats attackerStats = attacker.getCombinedStats();
	int attStat = attackerStats.m_stats[STAT_ATTACK];

	Pawn*& defender = tileTarget.mo_pawn;
	Stats defenderStats = defender->getCombinedStats();
	int defStat = defenderStats.m_stats[STAT_DEFENSE];

	int damage = CalcDamage(1, attStat, 16, defStat);

	defender->takeDirectDamage(damage);
	if (defender->isDead())
	{
		//SAFE_DELETE(defender);
		defender = nullptr;
	}
}
//////////////////////////////////////////////////////////////////////////
double Battle::PhysicalBaseDmg(double Stat, double DmCon)
{
	return (((Stat * Stat * Stat) / PHYBASEDMG1) + PHYBASEDMG1) * DmCon / PHYBASEDMG2;
}

double Battle::MagicalBaseDmg(double Stat, double DmCon)
{
	return DmCon * (((Stat * Stat) / MAGBASEDMG1) + DmCon) / MAGBASEDMG2;
}

double Battle::DefNum(double Def)
{
	return (((Def - DEFNUM1) * (Def - DEFNUM1)) / DEFNUM2) + DEFNUM3;
}

double Battle::ReducedDamage(double Dmg, double Def)
{
	return Dmg * Def / REDDMGNUM;
}

double Battle::FinalDamage(double BaseDmg, double Def)
{
	return BaseDmg * (FINALDMG1 - (Def * FINALDMG2 - (Def * Def) / FINALDMG3) / FINALDMG4) / FINALDMG1;
}

int Battle::CalcDamage(int attackType, double AttStat, double DmCon, double Def)
{
	double BaseDamage = 0;
	switch (attackType)
	{
	case 1:
		BaseDamage = PhysicalBaseDmg(AttStat, DmCon);
		break;
	case 2:
		BaseDamage = MagicalBaseDmg(AttStat, DmCon);
		break;
	}

	return (int)FinalDamage(ReducedDamage(BaseDamage, DefNum(Def)), Def);
}
