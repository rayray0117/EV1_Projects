#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
struct XMLNode;
//////////////////////////////////////////////////////////////////////////
enum StatName
{
	STAT_HEALTH,
	STAT_STAMINA,			//How much energy do I have to move.
	STAT_MOVEMENT,			//How many spaces I can move in a turn.
	STAT_SPEED,				//How often can I move compared to everyone else.
	STAT_ATTACK,
	STAT_DEFENSE,			//Reduces Physical Damage. May effect magical still debating on that
	STAT_MAGIC,
	STAT_SIGHT, //How far can I see
	NUM_STATS
};
// Remember focus on movement and setting up your next action when designing!
//////////////////////////////////////////////////////////////////////////
enum EquipSlot
{
	EQSLOT_NONE = -1,
	EQSLOT_ARMOR,		//Just gives stats bonuses
	EQSLOT_MOVEMENT,	//Will decide they way you move around the map
	EQSLOT_WEAPON,		//Will decide the types of attacks and spells you can use.
	NUM_EQSLOTS
};
//////////////////////////////////////////////////////////////////////////
class Stats
{
public:
	Stats();
	~Stats() {}
	
	void getStatFromNode(const XMLNode& node);
	
	const Stats operator+(const Stats& rightHandSide) const;
	void operator+=(const Stats& rightHandSide);
public:
	int m_stats[NUM_STATS];
};

//////////////////////////////////////////////////////////////////////////
// Inlines
inline Stats::Stats()
{
	for (uint i = 0; i < NUM_STATS; i++)
	{
		m_stats[i] = 0;
	}
}

inline const Stats Stats::operator+(const Stats& rightHandSide) const
{
	Stats result;
	for (uint i = 0; i < NUM_STATS; i++)
	{
		result.m_stats[i] = m_stats[i] + rightHandSide.m_stats[i];
	}
	return result;
}

inline void Stats::operator+=(const Stats& rightHandSide)
{
	for (uint i = 0; i < NUM_STATS; i++)
	{
		m_stats[i] += rightHandSide.m_stats[i];
	}
}