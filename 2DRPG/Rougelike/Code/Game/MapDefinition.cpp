#include "Game/MapDefinition.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/Console.hpp"

#include "Game/MapGenerator.hpp"
#include "Game/MapGeneratorFromFile.hpp"
#include "Game/MapGeneratorRoomsAndPaths.hpp"
#include "Game/MapGeneratorPerlin.hpp"
#include "Game/MapGeneratorCellularAutomata.hpp"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
const std::string GEN_NAME_ERROR = "GEN_NAME_ERROR";
//////////////////////////////////////////////////////////////////////////
MapDefinition::MapDefinition(const XMLNode& element)
	:DataDrivenDefinition(element)
{
	m_dimensions = ParseXMLAttribute(element, "dimensions", IntVector2(-1, -1));
	ASSERT_OR_DIE(m_dimensions != IntVector2(-1, -1), "ERROR: Map " + m_name + " didn't have dimensions assigned!");

	m_defaultTileName = ParseXMLAttribute(element, "defaultTile", (std::string)"ERROR: UNNAMED TILE!");
	ASSERT_OR_DIE(m_defaultTileName != "ERROR: UNNAMED TILE!", "ERROR: A Tile name was unassinged");

	m_sightBonus = ParseXMLAttribute(element, "sightBonus", m_sightBonus);

	m_bgm = ParseXMLAttribute(element, "BGM", m_bgm);

	int numGenerators = element.nChildNode("Generation");
	m_generators.reserve(numGenerators);
	for (int i = 0; i < numGenerators; i++)
	{
		XMLNode generatorNode = element.getChildNode("Generation", i);
		createGeneratorFromNode(generatorNode);
	}

	int numPawns = element.nChildNode("Pawn");
	m_pawnsToSpawn.reserve(numPawns);
	for (int i = 0; i < numPawns; i++)
	{
		XMLNode pawnNode = element.getChildNode("Pawn", i);
		m_pawnsToSpawn.push_back(new SpawnPawnHelper(pawnNode));
	}

	int numFeature = element.nChildNode("Feature");
	m_featuresToSpawn.reserve(numFeature);
	for (int i = 0; i < numFeature; i++)
	{
		XMLNode featureNode = element.getChildNode("Feature", i);
		m_featuresToSpawn.push_back(new SpawnFeatureHelper(featureNode));
	}
}

MapDefinition::~MapDefinition()
{
	for (uint i = 0; i < m_generators.size(); i++)
	{
		SAFE_DELETE(m_generators[i]);
	}
	m_generators.clear();

	for (uint i = 0; i < m_pawnsToSpawn.size(); i++)
	{
		SAFE_DELETE(m_pawnsToSpawn[i]);
	}
	m_pawnsToSpawn.clear();

	for (uint i = 0; i < m_featuresToSpawn.size(); i++)
	{
		SAFE_DELETE(m_featuresToSpawn[i]);
	}
	m_featuresToSpawn.clear();
}

//////////////////////////////////////////////////////////////////////////
void MapDefinition::createGeneratorFromNode(const XMLNode& node)
{
	std::string generationName = ParseXMLAttribute(node, "generator", GEN_NAME_ERROR);
	ASSERT_OR_DIE(generationName != GEN_NAME_ERROR, "ERROR: A generator type was never set!");

	if (generationName == "FromFile")
	{
		m_generators.push_back(new MapGeneratorFromFile(node));
		return;
	}
// 	if (generationName == "RoomsAndMazes")
// 	{
// 		m_generators.push_back(new MapGeneratorFromFile(node));
// 		return;
// 	}
	if (generationName == "RoomsAndPaths")
	{
		m_generators.push_back(new MapGeneratorRoomsAndPaths(node));
		return;
	}
	if (generationName == "Perlin")
	{
		m_generators.push_back(new MapGeneratorPerlin(node));
		return;
	}
	if (generationName == "CellAuto")
	{
		m_generators.push_back(new MapGeneratorCellularAutomata(node));
		return;
	}
}

////////////////////////////////////////////////////////////////////////// COMMANDS //////////////////////////////////////////////////////////////////////////
COMMAND(map_defs, "Lists the available map definitions")
{
	for (std::map<std::string, MapDefinition*>::iterator iterator = MapDefinition::s_registry.begin(); iterator != MapDefinition::s_registry.end(); iterator++)
	{
		g_theConsole->addMessage(iterator->second->m_name, Rgba(225, 128, 225, 255));
	}
}