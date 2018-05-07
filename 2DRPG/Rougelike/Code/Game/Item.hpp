#pragma once
#include "Game/ItemDefinition.hpp"
//////////////////////////////////////////////////////////////////////////
class Item
{
public:
	Item(ItemDefinition* definition);
	~Item() {}

public:
	ItemDefinition* mo_itemDef;
	const Stats& mo_stats; //Note: May want to do this differently in future.
};