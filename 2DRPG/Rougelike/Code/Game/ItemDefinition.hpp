#pragma once
#include "Engine/General/DataDrivenDefinition.hpp"

#include "Game/Stats.hpp"
//////////////////////////////////////////////////////////////////////////
class ItemDefinition : public DataDrivenDefinition<ItemDefinition>
{
	friend DataDrivenDefinition<ItemDefinition>;
public:
	~ItemDefinition() {};
	
public:
	Stats m_stats;
	EquipSlot m_equipSlot;
	// Move list?
private:
	ItemDefinition(const XMLNode& element);
};
