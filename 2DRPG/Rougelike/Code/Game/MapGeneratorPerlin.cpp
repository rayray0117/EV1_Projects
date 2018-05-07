#include "Game/MapGeneratorPerlin.hpp"

#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Noise.hpp"

#include "Game/Map.hpp"
#include "Game/Tile.hpp"
//////////////////////////////////////////////////////////////////////////
Condition::Condition(const XMLNode& element)
{
	ifNoise = ParseXMLAttribute(element, "range", Vector2::ZERO);
	changeToType = ParseXMLAttribute(element, "newType", changeToType);
	ifTile = ParseXMLAttribute(element, "ifTile", ifTile);
	chanceToRun = ParseXMLAttribute(element, "chanceToRun", chanceToRun);
}

const bool Condition::isInRange(float value) const
{
	if (value < ifNoise.x)
		return false;
	if (value > ifNoise.y)
		return false;

	return true;
}

void Condition::ChangeIfValidTile(Map* mapToEdit, const IntVector2& tileCoords)
{
	Tile* tile = mapToEdit->getTile(tileCoords);
	if (!tile) return; //Don't try to change a tile that doesn't exist
	if (ifTile.empty() || *tile == ifTile)
	{
		tile->changeType(changeToType);
	}
}

//////////////////////////////////////////////////////////////////////////
MapGeneratorPerlin::MapGeneratorPerlin(const XMLNode& element)
{
	m_scale = ParseXMLAttribute(element, "scale", m_scale);
	m_numOctaves = (uint)ParseXMLAttribute(element, "numOctaves", (int)m_numOctaves);
	m_octavePersistence = ParseXMLAttribute(element, "persistence", m_octavePersistence);
	m_octaveScale = ParseXMLAttribute(element, "octScale", m_octaveScale);
	m_renormalize = ParseXMLAttribute(element, "renorm", m_renormalize);
	m_seed = (uint)ParseXMLAttribute(element, "seed", (int)m_seed);

	m_dimensions = ParseXMLAttribute(element, "dimensions", -IntVector2::ONE);
	m_offset = ParseXMLAttribute(element, "offset", IntVector2::ZERO);

	int numRules = element.nChildNode("Rule");
	for(int i = 0; i < numRules; i++)
	{
		XMLNode ruleNode = element.getChildNode("Rule", i);
		m_rules.push_back(Condition(ruleNode));
	}
}

void MapGeneratorPerlin::Generate(Map* mapToEdit)
{
	if (m_dimensions == -IntVector2::ONE)
	{
		m_dimensions = mapToEdit->m_dimensions;
	}

	for (int y = 0; y < m_dimensions.y; y++)
	{
		for (int x = 0; x < m_dimensions.x; x++)
		{
			IntVector2 position = IntVector2(x, y) + m_offset;
			float noiseValue = Compute2dPerlinNoise((float)position.x, (float)position.y, m_scale, m_numOctaves, m_octavePersistence, m_octaveScale, m_renormalize, m_seed);

			for (Condition current : m_rules)
			{
				if (!isPercentChance(current.chanceToRun))
					return;
				if (current.isInRange(noiseValue))
				{
					current.ChangeIfValidTile(mapToEdit, position);
				}
			}
		}
	}
}

