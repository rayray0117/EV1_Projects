#include "Game/Item.hpp"
//////////////////////////////////////////////////////////////////////////
Item::Item(ItemDefinition* definition)
	: mo_itemDef(definition)
	, mo_stats(mo_itemDef->m_stats)
{
}