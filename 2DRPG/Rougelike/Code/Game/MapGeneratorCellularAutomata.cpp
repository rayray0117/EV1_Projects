#include "Game/MapGeneratorCellularAutomata.hpp"

#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Noise.hpp"

#include "Game/Map.hpp"
#include "Game/Tile.hpp"
//////////////////////////////////////////////////////////////////////////
CellRule::CellRule(const XMLNode& element)
{
	range = ParseXMLAttribute(element, "range", -IntVector2::ONE);
	changeToType = ParseXMLAttribute(element, "newType", changeToType);
	ifMyTile = ParseXMLAttribute(element, "ifMyTile", ifMyTile);
	ifThierTile = ParseXMLAttribute(element, "ifThierTile", ifThierTile);
	chanceToRun = ParseXMLAttribute(element, "chanceToRun", chanceToRun);
}

const bool CellRule::isInRange(int value) const
{
	if (value < range.x)
		return false;
	if (value > range.y)
		return false;

	return true;
}

void CellRule::ChangeIfValidTile(Map* mapToEdit, const IntVector2& tileCoords, std::vector<std::string>& tempMap)
{
	Tile* tile = mapToEdit->getTile(tileCoords);
	if (!tile) return; //Don't try to change a tile that doesn't exist
	if (ifMyTile.empty() || *tile == ifMyTile)
	{
		int tileIndex = mapToEdit->getTileIndex(tileCoords);
		tempMap[tileIndex] = changeToType;
	}
}

//////////////////////////////////////////////////////////////////////////
MapGeneratorCellularAutomata::MapGeneratorCellularAutomata(const XMLNode& element)
{
	m_dimensions = ParseXMLAttribute(element, "dimensions", -IntVector2::ONE);
	m_offset = ParseXMLAttribute(element, "offset", IntVector2::ZERO);
	m_numIterations = ParseXMLAttribute(element, "numIterations", m_numIterations);

	int numRules = element.nChildNode("Rule");
	for (int i = 0; i < numRules; i++)
	{
		XMLNode ruleNode = element.getChildNode("Rule", i);
		m_rules.push_back(CellRule(ruleNode));
	}
}

void MapGeneratorCellularAutomata::Generate(Map* mapToEdit)
{
	//This looks awful and can be broken down more but I don't care because it was very boring to do and I want to be done with it.
	if (m_dimensions == -IntVector2::ONE)
	{
		m_dimensions = mapToEdit->m_dimensions;
	}

	m_tempMap.resize(m_dimensions.x * m_dimensions.y);
	for (int i = 0; i < m_numIterations; i++)
	{
		for (int y = 0; y < m_dimensions.y; y++)
		{
			for (int x = 0; x < m_dimensions.x; x++)
			{
				IntVector2 position = IntVector2(x, y) + m_offset;


				for (CellRule currentRule : m_rules)
				{
					if (!isPercentChance(currentRule.chanceToRun))
						return;

					std::vector<Tile*> neighbors;
					mapToEdit->getNeighbors_SquareSingle(position, neighbors, 1);
					int numNeighborsOfOtherType = 0;
					for (Tile* currentNeighborTile : neighbors)
					{
						if(currentNeighborTile == nullptr)
							continue;

						if (*currentNeighborTile == currentRule.ifThierTile)
							++numNeighborsOfOtherType;
					}

					if (currentRule.isInRange(numNeighborsOfOtherType))
					{
						currentRule.ChangeIfValidTile(mapToEdit, position, m_tempMap);
					}
				}
			}
		}


		for (int y = 0; y < m_dimensions.y; y++)
		{
			for (int x = 0; x < m_dimensions.x; x++)
			{
				IntVector2 position = IntVector2(x, y) + m_offset;

				Tile* currentTile = mapToEdit->getTile(position);

				int tempMapIndex = (y * m_dimensions.x) + x;
				currentTile->changeType(m_tempMap[tempMapIndex]);
			}
		}
	}
}
