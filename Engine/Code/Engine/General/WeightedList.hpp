#pragma once
#include "Engine/Math/MathUtils.hpp"
#include <vector>
TODO("This doesn't quite seem like the way I'd use it. Come back to this when I have a better idea of what I want to do.");
//////////////////////////////////////////////////////////////////////////
template <typename T_ListItemType>
class WeightedList
{
public:
	WeightedList() {};
	~WeightedList() {};
	
	void addItem(const T_ListItemType& item, float weight = 1.f);
	void removeItem(const T_ListItemType& item);
	void clear() { m_items.clear(); m_weights.clear(); m_totalWeight = 0.f; };
	void changeItemWeight(const T_ListItemType& item, float weight);

	const T_ListItemType& getRandomItem() const;
private:
	std::vector<T_ListItemType> m_items;
	std::vector<float> m_weights;
	float m_totalWeight = 0.f;

private:
	const uint getItemIndex(const T_ListItemType& item) const;
};
//////////////////////////////////////////////////////////////////////////
template <typename T_ListItemType>
void WeightedList<T_ListItemType>::addItem(const T_ListItemType& item, float weight /*= 1.f*/)
{
	m_items.push_back(item);
	m_weights.push_back(weight);
}

template <typename T_ListItemType>
void WeightedList<T_ListItemType>::removeItem(const T_ListItemType& item)
{
	uint itemsIndex = getItemIndex(item);
	m_items.erase(m_items.begin() + itemsIndex);
	m_weights.erase(m_weights.begin() + itemsIndex);
}

template <typename T_ListItemType>
void WeightedList<T_ListItemType>::changeItemWeight(const T_ListItemType& item, float weight)
{
	uint itemsIndex = getItemIndex(item);
	m_weights[itemsIndex] = weight;
}

template <typename T_ListItemType>
const T_ListItemType& WeightedList<T_ListItemType>::getRandomItem() const
{
	uint itemIndex = getRandomIntLessThan(m_items.size());
	return m_items[itemIndex];
}


template <typename T_ListItemType>
const uint WeightedList<T_ListItemType>::getItemIndex(const T_ListItemType& item) const
{
	uint itemsIndex = 0;
	for (; itemsIndex < m_items.size(); itemsIndex++)
	{
		if (m_items[itemsIndex] == item)
			return itemsIndex;
	}

	ERROR_AND_DIE("Could not find the Item in the weighted list!");
}

