#pragma once
#include <vector>
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class Item;
//////////////////////////////////////////////////////////////////////////
class Inventory
{
public:
	Inventory() {}
	~Inventory();

	void DebugRender() const;

	void AddItem(Item* newItem);
	Item* GetItem(uint itemIndex);
	Item* RemoveItem(uint itemIndex);
	void DeleteItem(uint itemIndex);

	//Operators
	void operator+=(const Inventory& rightHandSide);
	const Inventory operator+(const Inventory& rightHandSide) const;
public:
	std::vector<Item*> m_items;
};