#include "Game/Inventory.hpp"
#include "Game/Item.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/GameCommon.hpp"
//////////////////////////////////////////////////////////////////////////
Inventory::~Inventory()
{
    for (uint i = 0; i< m_items.size(); i++)
    {
        SAFE_DELETE(m_items[i]);
    }
}
//////////////////////////////////////////////////////////////////////////
void Inventory::DebugRender() const
{
	BitmapFont* f = g_renderer->CreateOrGetBitmapFont(g_bitmapFont);

	g_renderer->DrawText2D(Vector2::ZERO, "INVENTORY", 25.f, Rgba::WHITE, 1.f, f);
	g_renderer->translate2D(Vector2(0.f, -25.f));
	for (Item* current : m_items)
	{
		g_renderer->DrawText2D(Vector2::ZERO, Stringf("%s", current->mo_itemDef->m_name.c_str()), 20.f, Rgba::WHITE, 1.f, f);
		g_renderer->translate2D(Vector2(0.f, -20.f));
	}
}

//////////////////////////////////////////////////////////////////////////
void Inventory::AddItem(Item* newItem)
{
	m_items.push_back(newItem);
}

Item* Inventory::GetItem(uint itemIndex)
{
	return m_items[itemIndex];
}

Item* Inventory::RemoveItem(uint itemIndex)
{
	Item* removedItem = m_items[itemIndex];
	m_items[itemIndex] = nullptr;
	m_items.erase(m_items.begin() + itemIndex);
	return removedItem;
}

void Inventory::DeleteItem(uint itemIndex)
{
	SAFE_DELETE(m_items[itemIndex]);
	m_items.erase(m_items.begin() + itemIndex);
}

//////////////////////////////////////////////////////////////////////////
void Inventory::operator+=(const Inventory& rightHandSide)
{
	m_items.insert(m_items.end(), rightHandSide.m_items.begin(), rightHandSide.m_items.end());
}

const Inventory Inventory::operator+(const Inventory& rightHandSide) const
{
	Inventory result;
	result.m_items = m_items;
	result.m_items.insert(result.m_items.end(), rightHandSide.m_items.begin(), rightHandSide.m_items.end());

	return result;
}
