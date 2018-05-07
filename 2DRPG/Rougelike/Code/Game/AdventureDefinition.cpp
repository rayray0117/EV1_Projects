#include "Game/AdventureDefinition.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Game/GameCommon.hpp"
//////////////////////////////////////////////////////////////////////////a
AdventureDefinition::AdventureDefinition(const XMLNode& element)
	:DataDrivenDefinition(element)
{
	m_title = ParseXMLAttribute(element, "title", m_title);
	m_sprite = ParseXMLAttribute(element, "sprites", std::string(SPRITE_BITMAP_FONT_NAME));
	m_font = ParseXMLAttribute(element, "font", std::string(DEFAULT_BITMAP_FONT_NAME));
	
	int numMaps = element.nChildNode("Map");
	ASSERT_OR_DIE(numMaps != 0, "Don't have any maps for your adventure!");
	for (int i = 0; i < numMaps; i++)
	{
		XMLNode n = element.getChildNode("Map", i);
		std::string mapName = ParseXMLAttribute(n, "name", std::string(""));
		std::string mapDef = ParseXMLAttribute(n, "def", std::string(""));
		ASSERT_OR_DIE(!mapName.empty() && !mapDef.empty(), "Didn't give map names or defs when creating your adventure. Both are needed!");
		m_maps.push_back(new MapCreationHelper(n));
	}

	m_startMapName = ParseXMLAttribute(element, "startMap", m_maps[0]->m_mapName);
}
////////////////////////////////////////////////////////////////////////// //////////////////////////////////////////////////////////////////////////
ExitSpawnHelper::ExitSpawnHelper(const XMLNode& element)
{
	feature = new SpawnFeatureHelper(element);
	exitPath = ParseXMLAttribute(element, "toMap", exitPath);
	ASSERT_OR_DIE(!exitPath.empty(), "You created an exit that goes to no where!");
}

MapCreationHelper::MapCreationHelper(const XMLNode& element)
{
	m_mapName = ParseXMLAttribute(element, "name", m_mapName);
	m_mapDef = ParseXMLAttribute(element, "def", m_mapDef);
	TODO("Add asserts for when a map doesn't have a name or def");

	int numExits = element.nChildNode("Exit");
	for (int i = 0; i < numExits; i++)
	{
		XMLNode n = element.getChildNode("Exit", i);
		m_exits.push_back(new ExitSpawnHelper(n));
	}

	int numPawns = element.nChildNode("Pawn");
	for (int i = 0; i < numPawns; i++)
	{
		XMLNode n = element.getChildNode("Pawn", i);
		m_pawns.push_back(new SpawnPawnHelper(n));
	}

	int numFeatures = element.nChildNode("Feature");
	for (int i = 0; i < numFeatures; i++)
	{
		XMLNode n = element.getChildNode("Feature", i);
		m_features.push_back(new SpawnFeatureHelper(n));
	}

	int numEntrances = element.nChildNode("Enter");
	if (numEntrances > 0)
	{
		XMLNode n = element.getChildNode("Enter", 0);
		m_entrance = new SpawnHelper(n);
	}
		
}
