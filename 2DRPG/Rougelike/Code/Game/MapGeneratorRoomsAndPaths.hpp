#pragma once
#include "Game/MapGenerator.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class MapGeneratorRoomsAndPaths : public MapGenerator
{
public:
	MapGeneratorRoomsAndPaths(const XMLNode& element);
	~MapGeneratorRoomsAndPaths();
	
	void Generate(Map* mapToEdit);
public:
	std::string m_roomFloor;
	std::string m_roomWall;
	std::string m_pathFloor;
	IntVector2 m_roomHeights;
	IntVector2 m_roomWidths;
	IntVector2 m_roomCounts;
	int m_numberOfAllowedOverlaps = 0;

private:
	void AddRooms(Map* mapToEdit);
	void addRoomTiles(Map* mapToEdit, const AABB2& room, const std::string& tileType);
	void AddPaths(Map* mapToEdit);
	void addPath(Map* mapToEdit, const AABB2& roomFrom, const AABB2& roomTo);
	void AddFloors(Map* mapToEdit);

	void PlaceTilesIntoMap(Map* mapToEdit);

	void addTile(Map* mapToEdit, const IntVector2& pos, const std::string& tileType);

	const bool doesOverlap(const AABB2& possibleRoom);
	
private:
	IntVector2 m_dimensions;
	std::vector<std::string> m_mapTiles;
	std::vector<AABB2> m_rooms;
};