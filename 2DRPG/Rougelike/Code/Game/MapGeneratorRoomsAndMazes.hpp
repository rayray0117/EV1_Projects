#pragma once
#include "Game/MapGenerator.hpp"

#include "Engine/Core/EngineBase.hpp"
#include <string>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2.hpp"
//////////////////////////////////////////////////////////////////////////
struct XMLNode;
//////////////////////////////////////////////////////////////////////////
class MapGeneratorRoomsAndMazes : public MapGenerator
{
public:
	MapGeneratorRoomsAndMazes(const XMLNode& element);
	~MapGeneratorRoomsAndMazes() {};
	
	void Generate(Map* mapToEdit);
public:
	int numRoomTries;
	IntVector2 m_maxDimensions;
	AABB2 m_roomSizes;
	float m_extraConnectChance;
	float m_windingPercent;
	std::string m_fillTile;
	std::string m_openTile;
private:
	void AddRooms(Map* mapToEdit);
	void GrowMaze(Map* mapToEdit, const IntVector2& position);
	void ConnectRegions(Map* mapToEdit);
	void RemoveDeadEnds(Map* mapToEdit);

	uint getNumExits(Map* mapToEdit, int index) const;
private:
	//std::vector<int> m_map
};