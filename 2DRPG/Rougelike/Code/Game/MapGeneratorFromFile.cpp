#include "Game/MapGeneratorFromFile.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"

#include "Game/Map.hpp"
//////////////////////////////////////////////////////////////////////////
const std::string TYPE_ERROR = "NO TYPE FOUND!";
const std::string FILE_PATH_ERROR = "NO TYPE FOUND!";
const std::string ROW_TILES_ERROR = "ROW_TILES_ERROR!";
//////////////////////////////////////////////////////////////////////////
MapGeneratorFromFile::MapGeneratorFromFile(const XMLNode& element)
{
	m_filePath = ParseXMLAttribute(element, "file", FILE_PATH_ERROR);
	ASSERT_OR_DIE(m_filePath != FILE_PATH_ERROR, "ERROR: Generate map from file's path was not set!");

	m_offset = ParseXMLAttribute(element, "offset", IntVector2::ZERO);
	m_reversed = ParseXMLAttribute(element, "reverse", m_reversed);

	XMLNode fileRoot = XMLNode::openFileHelper(m_filePath.c_str());

	XMLNode legendNode = fileRoot.getChildNode("Legend");
	int numTileTypes = legendNode.nChildNode();
	for (int i = 0; i < numTileTypes; i++)
	{
		XMLNode tileNode = legendNode.getChildNode(i);
		char key = ParseXMLAttribute(tileNode, "key", char(0));
		ASSERT_OR_DIE(key != 0, "ERROR: Key not set in the legend for " + m_filePath);
		std::string type = ParseXMLAttribute(tileNode, "type", TYPE_ERROR);
		ASSERT_OR_DIE(type != TYPE_ERROR, "ERROR: Tile type not defined in legend for " + m_filePath);
		m_legend[key] = type;
	}

	XMLNode tilesNode = fileRoot.getChildNode("Tiles");
	int numRows = tilesNode.nChildNode();
	m_rows.reserve(numRows);

	if (m_reversed)
	{
		for (int i = 0; i <= numRows; i++) 
		{
			XMLNode row = tilesNode.getChildNode(i);
			std::string tiles = ParseXMLAttribute(row, "tiles", ROW_TILES_ERROR);
			ASSERT_OR_DIE(tiles != ROW_TILES_ERROR, "ERROR: Tiles not set for rows in " + m_filePath);
			m_rows.push_back(tiles);
		}
	}
	else
	{
		for (int i = numRows - 1; i >= 0; i--) //Read in reverse order so it'll look right when loaded in.
		{
			XMLNode row = tilesNode.getChildNode(i);
			std::string tiles = ParseXMLAttribute(row, "tiles", ROW_TILES_ERROR);
			ASSERT_OR_DIE(tiles != ROW_TILES_ERROR, "ERROR: Tiles not set for rows in " + m_filePath);
			m_rows.push_back(tiles);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void MapGeneratorFromFile::Generate(Map* mapToEdit)
{
	for (int y = 0; y < (int)m_rows.size(); y++)
	{
		for (int x = 0; x < (int)m_rows[y].size(); x++)
		{
			IntVector2 pos(x, y);
			std::string tileType = getTileTypeFromLegendKey(m_rows[y][x]);  //Hmmm so I ended up with a 2D array...
			Tile* tile = mapToEdit->getTile(pos + m_offset);
			tile->changeType(tileType);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
const std::string MapGeneratorFromFile::getTileTypeFromLegendKey(char key) const
{
	auto found = m_legend.find(key);
	if (found == m_legend.end())
	{
		ERROR_AND_DIE("Tried to access a key that was not in the legened!");
	}
	
	return found->second;
}
