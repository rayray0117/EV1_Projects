#include "Game/SpawnHelpers.hpp"

#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
//////////////////////////////////////////////////////////////////////////
SpawnAtHelp::SpawnAtHelp(const XMLNode& element)
{
	m_force = ParseXMLAttribute(element, "force", m_force);
	m_tileCoords = ParseXMLAttribute(element, "tileCoords", -IntVector2::ONE);
	ASSERT_OR_DIE(m_tileCoords != IntVector2(-1, -1), "ERROR: A map has a SpawnAt with no tile coordinates assigned!");
}


SpawnOnHelp::SpawnOnHelp(const XMLNode& element)
{
	m_numberRangeToSpawn = ParseXMLAttribute(element, "number", m_numberRangeToSpawn);
	m_tileType = ParseXMLAttribute(element, "tile", m_tileType);
	ASSERT_OR_DIE(!m_tileType.empty(), "ERROR: A map has a SpawnOn with no tile type assigned!");
	m_triesBeforeQuiting = ParseXMLAttribute(element, "tries", m_triesBeforeQuiting);
}



SpawnInHelp::SpawnInHelp(const XMLNode& element)
{
	m_numberRangeToSpawn = ParseXMLAttribute(element, "number", m_numberRangeToSpawn);
	m_tileFaction = ParseXMLAttribute(element, "faction", m_tileFaction);
	ASSERT_OR_DIE(!m_tileFaction.empty(), "ERROR: A map has a SpawnIn with no faction assigned!");
	m_triesBeforeQuiting = ParseXMLAttribute(element, "tries", m_triesBeforeQuiting);
}



DefAndPercent::DefAndPercent(const XMLNode& element)
{
	definitionName = ParseXMLAttribute(element, "name", definitionName);
	ASSERT_OR_DIE(!definitionName.empty(), "ERROR: A map has a Item/Loot with no type name assigned!");
	percentChanceToBeAdded = ParseXMLAttribute(element, "chance", percentChanceToBeAdded);
}


SpawnInvetoryHelp::SpawnInvetoryHelp(const XMLNode& element)
{
	int numLootNodes = element.nChildNode("Loot");
	int numItemNodes = element.nChildNode("Item");

	for (int lootIndex = 0; lootIndex < numLootNodes; ++lootIndex)
	{
		m_loots.push_back(DefAndPercent(element.getChildNode("Loot", lootIndex)));
	}
	for (int itemIndex = 0; itemIndex < numItemNodes; ++itemIndex)
	{
		m_items.push_back(DefAndPercent(element.getChildNode("Item", itemIndex)));
	}
}

SpawnEquipmentHelp::SpawnEquipmentHelp(const XMLNode& element)
{
	m_auto = ParseXMLAttribute(element, "auto", m_auto);
	/* DO NOTHING */
	//Don't handle equipment yet till I really know how I should handle it.
}


SpawnPawnHelper::SpawnPawnHelper(const XMLNode& element)
	: m_invetoryToSpawn(nullptr)
	, m_equipmentToSpawn(nullptr)
{
	m_pawnType = ParseXMLAttribute(element, "name", m_pawnType);
	ASSERT_OR_DIE(!m_pawnType.empty(), "ERROR: A map has pawn with no name assigned!");
	m_controllerId = ParseXMLAttribute(element, "controller", m_controllerId);

	int numSpawnAts = element.nChildNode("SpawnAt");
	int numSpawnOns = element.nChildNode("SpawnOn");
	int numSpawnIns = element.nChildNode("SpawnIn");

	for (int i = 0; i < numSpawnAts; i++)
	{
		m_spawnAts.push_back(SpawnAtHelp(element.getChildNode("SpawnAt", i)));
	}
	for (int i = 0; i < numSpawnOns; i++)
	{
		m_spawnOns.push_back(SpawnOnHelp(element.getChildNode("SpawnOn", i)));
	}
	for (int i = 0; i < numSpawnIns; i++)
	{
		m_spawnIns.push_back(SpawnInHelp(element.getChildNode("SpawnIn", i)));
	}

	XMLNode inventoryNode = element.getChildNode("Inventory");
	if (!inventoryNode.isEmpty())
	{
		m_invetoryToSpawn = new SpawnInvetoryHelp(inventoryNode);
	}

	XMLNode equipNode = element.getChildNode("Equip");
	if (!equipNode.isEmpty())
	{
		m_equipmentToSpawn = new SpawnEquipmentHelp(equipNode);
	}
}


SpawnFeatureHelper::SpawnFeatureHelper(const XMLNode& element)
	: m_invetoryToSpawn(nullptr)
{
	m_featureType = ParseXMLAttribute(element, "name", m_featureType);
	ASSERT_OR_DIE(!m_featureType.empty(), "ERROR: A map has a feature with no name assigned!");

	int numSpawnAts = element.nChildNode("SpawnAt");
	int numSpawnOns = element.nChildNode("SpawnOn");
	int numSpawnIns = element.nChildNode("SpawnIn");

	for (int i = 0; i < numSpawnAts; i++)
	{
		m_spawnAts.push_back(SpawnAtHelp(element.getChildNode("SpawnAt", i)));
	}
	for (int i = 0; i < numSpawnOns; i++)
	{
		m_spawnOns.push_back(SpawnOnHelp(element.getChildNode("SpawnOn", i)));
	}
	for (int i = 0; i < numSpawnIns; i++)
	{
		m_spawnIns.push_back(SpawnInHelp(element.getChildNode("SpawnIn", i)));
	}

	XMLNode inventoryNode = element.getChildNode("Inventory");
	if (!inventoryNode.isEmpty())
	{
		m_invetoryToSpawn = new SpawnInvetoryHelp(inventoryNode);
	}
}

////////////////////////////////////////////////////////////////////////// Spawn helpers //////////////////////////////////////////////////////////////////////////
SpawnNearPawnHelp::SpawnNearPawnHelp(const XMLNode& element)
{
	m_pawnType = ParseXMLAttribute(element, "pawnType", m_pawnType);
}


SpawnNearFactionHelp::SpawnNearFactionHelp(const XMLNode& element)
{
	m_factionName = ParseXMLAttribute(element, "factionName", m_factionName);
}


SpawnHelper::SpawnHelper(const XMLNode& element)
{
	int numNearPawnNodes = element.nChildNode("SpawnNearPawn");
	int numNearFactionNodes = element.nChildNode("SpawnNearFaction");
	int numAtNodes = element.nChildNode("SpawnAt");
	int numOnNodes = element.nChildNode("SpawnOn");
	int numInNodes = element.nChildNode("SpawnIn");

	for (int i = 0; i < numNearPawnNodes; i++)
	{
		XMLNode n = element.getChildNode("SpawnNearPawn", i);
		m_nearP.push_back(new SpawnNearPawnHelp(n));
	}

	for (int i = 0; i < numNearFactionNodes; i++)
	{
		XMLNode n = element.getChildNode("SpawnNearFaction", i);
		m_nearP.push_back(new SpawnNearPawnHelp(n));
	}

	for (int i = 0; i < numAtNodes; i++)
	{
		XMLNode n = element.getChildNode("SpawnAt", i);
		m_nearP.push_back(new SpawnNearPawnHelp(n));
	}

	for (int i = 0; i < numOnNodes; i++)
	{
		XMLNode n = element.getChildNode("SpawnOn", i);
		m_nearP.push_back(new SpawnNearPawnHelp(n));
	}

	for (int i = 0; i < numInNodes; i++)
	{
		XMLNode n = element.getChildNode("SpawnIn", i);
		m_nearP.push_back(new SpawnNearPawnHelp(n));
	}
}

SpawnHelper::~SpawnHelper()
{
	for (uint i = 0; i < m_nearP.size(); i++)
	{
		SAFE_DELETE(m_nearP[i]);
	}
	for (uint i = 0; i < m_at.size(); i++)
	{
		SAFE_DELETE(m_at[i]);
	}
	for (uint i = 0; i < m_in.size(); i++)
	{
		SAFE_DELETE(m_in[i]);
	}
	for (uint i = 0; i < m_on.size(); i++)
	{
		SAFE_DELETE(m_on[i]);
	}
	for (uint i = 0; i < m_nearF.size(); i++)
	{
		SAFE_DELETE(m_nearF[i]);
	}
}
