#include "Game/MapGeneratorRoomsAndMazes.hpp"

#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"

#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
//////////////////////////////////////////////////////////////////////////
MapGeneratorRoomsAndMazes::MapGeneratorRoomsAndMazes(const XMLNode& /*element*/)
{

}
//////////////////////////////////////////////////////////////////////////
void MapGeneratorRoomsAndMazes::Generate(Map* mapToEdit)
{
	m_maxDimensions = mapToEdit->m_dimensions;

	//Force our dimensions to work with to be odd
	if (m_maxDimensions.x % 2 == 0)
		m_maxDimensions.x -= 1;
	if (m_maxDimensions.y % 2 == 0)
		m_maxDimensions.y -= 1;

	//Fill with wall?

	AddRooms(mapToEdit);

	for (int y = 1; y < m_maxDimensions.y; y += 2)
	{
		for (int x = 0; x < m_maxDimensions.x; x += 2)
		{
			IntVector2 pos(x, y);
			if(mapToEdit->getTile(pos)->mo_tileDef->m_isSolid)
				GrowMaze(mapToEdit, pos);
		}
	}

	ConnectRegions(mapToEdit);
	RemoveDeadEnds(mapToEdit);
}

//////////////////////////////////////////////////////////////////////////
void MapGeneratorRoomsAndMazes::AddRooms(Map* /*mapToEdit*/)
{

}

void MapGeneratorRoomsAndMazes::GrowMaze(Map* /*mapToEdit*/, const IntVector2& position)
{
	std::vector<IntVector2> cells;
	IntVector2 lastDirection;


	cells.push_back(position);
	while (!cells.empty())
	{
		IntVector2 cell = cells.back();

	}
}

void MapGeneratorRoomsAndMazes::ConnectRegions(Map* /*mapToEdit*/)
{

}

void MapGeneratorRoomsAndMazes::RemoveDeadEnds(Map* mapToEdit)
{
	bool done = false;

	int numberOfTilesToLookAt = m_maxDimensions.x * m_maxDimensions.y;

	while (!done)
	{
		for (int index = 0; index < numberOfTilesToLookAt; index++)
		{
			if(mapToEdit->m_tiles[index].mo_tileDef->m_isSolid)
				continue;

			uint exits = getNumExits(mapToEdit, index);

			if(exits != 1)
				continue;

			done = false;
			mapToEdit->m_tiles[index].changeType(m_fillTile);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
uint MapGeneratorRoomsAndMazes::getNumExits(Map* mapToEdit, int index) const
{
	uint exits = 0;

	IntVector2 tilePos = mapToEdit->getTileCoords(index);

	IntVector2 rightTile = tilePos + IntVector2(1, 0);
	IntVector2 leftTile = tilePos + IntVector2(-1, 0);
	IntVector2 upTile = tilePos + IntVector2(0, -1);
	IntVector2 downTile = tilePos + IntVector2(0, -1);

	if (!mapToEdit->isTileSolid(rightTile))
		exits++;
	if (!mapToEdit->isTileSolid(leftTile))
		exits++;
	if (!mapToEdit->isTileSolid(upTile))
		exits++;
	if (!mapToEdit->isTileSolid(downTile))
		exits++;

	return exits;
}
