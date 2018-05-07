#pragma once
#include "Engine/General/DataDrivenDefinition.hpp"

#include "Game/Stats.hpp"
#include "Game/Sprite.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class Behavior;
//////////////////////////////////////////////////////////////////////////
class PawnDefinition : public DataDrivenDefinition<PawnDefinition>
{
	friend DataDrivenDefinition<PawnDefinition>;
public:
	~PawnDefinition();
	
public:
	Sprite m_sprite;
	Stats m_initialStats;
	std::string m_faction;
	std::vector<Behavior*> m_behaviors;
	std::string m_tookDamageSound;
private:
	PawnDefinition(const XMLNode& element);
};
