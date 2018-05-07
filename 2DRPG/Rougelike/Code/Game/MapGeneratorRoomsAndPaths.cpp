#include "Game/MapGeneratorRoomsAndPaths.hpp"

#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Game/Map.hpp"
//////////////////////////////////////////////////////////////////////////
constexpr uint MAX_NUM_TRIES = 600;
//////////////////////////////////////////////////////////////////////////
MapGeneratorRoomsAndPaths::MapGeneratorRoomsAndPaths(const XMLNode& element)
{
	XMLNode roomsNode = element.getChildNode("Rooms");

	m_roomCounts = ParseXMLAttribute(roomsNode, "maxTries", m_roomCounts);
	m_roomWidths = ParseXMLAttribute(roomsNode, "widths", m_roomWidths);
	m_roomHeights = ParseXMLAttribute(roomsNode, "heights", m_roomHeights);
	m_roomFloor = ParseXMLAttribute(roomsNode, "floor", m_roomFloor);
	m_roomWall = ParseXMLAttribute(roomsNode, "wall", m_roomWall);

	XMLNode pathsNode = element.getChildNode("Paths");
	m_pathFloor = ParseXMLAttribute(pathsNode, "floor", m_pathFloor);
}

MapGeneratorRoomsAndPaths::~MapGeneratorRoomsAndPaths()
{

}

void MapGeneratorRoomsAndPaths::Generate(Map* mapToEdit)
{
	m_dimensions = mapToEdit->m_dimensions;

	m_mapTiles.clear();
	m_mapTiles.resize(m_dimensions.x * m_dimensions.y);

	AddRooms(mapToEdit);
	AddPaths(mapToEdit);
	AddFloors(mapToEdit);

	PlaceTilesIntoMap(mapToEdit);
}

void MapGeneratorRoomsAndPaths::AddRooms(Map* mapToEdit)
{
	m_rooms.clear();
	int numOfOverlaps = 0;
	int numberOfRooms = getRandomIntInRange(m_roomCounts.x, m_roomCounts.y);
	for (uint tryNumber = 0; tryNumber < MAX_NUM_TRIES; tryNumber++)
	{
		int width = getRandomIntInRange(m_roomWidths.x, m_roomWidths.y);
		int height = getRandomIntInRange(m_roomHeights.x, m_roomHeights.y);

		int xPosMax = ((m_dimensions.x - 1) - width);
		int yPosMax = ((m_dimensions.y - 1) - height);
		int xPos = getRandomIntInRange(0, xPosMax);
		int yPos = getRandomIntInRange(0, yPosMax);

		AABB2 possibleRoom((float)xPos, (float)yPos, (float)(xPos + width), (float)(yPos + height));

		bool overlapped = doesOverlap(possibleRoom);
		if (overlapped && numOfOverlaps >= m_numberOfAllowedOverlaps)
		{
			continue;
		}
		else if (overlapped)
		{
			numOfOverlaps++;
		}
		m_rooms.push_back(possibleRoom);
		addRoomTiles(mapToEdit, possibleRoom, m_roomWall);

		if ((int)m_rooms.size() >= numberOfRooms)
			break;
	}
}

void MapGeneratorRoomsAndPaths::addRoomTiles(Map* mapToEdit, const AABB2& room, const std::string& tileType)
{
	for (int y = (int)room.mins.y; y <= (int)room.maxs.y; y++)
	{
		for (int x = (int)room.mins.x; x <= (int)room.maxs.x; x++)
		{
			addTile(mapToEdit, IntVector2(x, y), tileType);
		}
	}
}

void MapGeneratorRoomsAndPaths::AddPaths(Map* mapToEdit)
{
	for (uint roomIndex = 0; roomIndex < m_rooms.size(); roomIndex++)
	{
		if (roomIndex == m_rooms.size() - 1)
		{
			addPath(mapToEdit, m_rooms[roomIndex], m_rooms[0]);
		}
		else
		{
			addPath(mapToEdit, m_rooms[roomIndex], m_rooms[roomIndex+1]);
		}
	}
}

void MapGeneratorRoomsAndPaths::addPath(Map* mapToEdit, const AABB2& roomFrom, const AABB2& roomTo)
{
	IntVector2 startPos(roomFrom.calcCenter());
	IntVector2 endPos(roomTo.calcCenter());

	IntVector2 currentPos = startPos;
	addTile(mapToEdit, currentPos, m_pathFloor);

	while (currentPos != endPos)
	{
		IntVector2 nextDirection = currentPos.getRandomDirectionToward(endPos);
		currentPos += nextDirection;

		addTile(mapToEdit, currentPos, m_pathFloor);
	}
}

void MapGeneratorRoomsAndPaths::AddFloors(Map* mapToEdit)
{
	for (AABB2 room : m_rooms)
	{
		room.AddPaddingToSides(-1.f, -1.f);
		addRoomTiles(mapToEdit, room, m_roomFloor);
	}
}

void MapGeneratorRoomsAndPaths::PlaceTilesIntoMap(Map* mapToEdit)
{
	for (uint i = 0; i < m_mapTiles.size(); i++)
	{
		if(m_mapTiles[i].empty())
			continue;

		mapToEdit->m_tiles[i].changeType(m_mapTiles[i]);
	}
}

void MapGeneratorRoomsAndPaths::addTile(Map* mapToEdit, const IntVector2& pos, const std::string& tileType)
{
	int tileIndex = mapToEdit->getTileIndex(pos);
	m_mapTiles[tileIndex] = tileType;
}

const bool MapGeneratorRoomsAndPaths::doesOverlap(const AABB2& possibleRoom)
{
	for (uint i = 0; i < m_rooms.size(); i++)
	{
		if (doAABBsOverlap(possibleRoom, m_rooms[i]))
			return true;
	}

	return false;
}
