#pragma once
#include "Game/Map.hpp"
//////////////////////////////////////////////////////////////////////////
struct OpenNode
{
	friend class PathGenerator;
private:
	Tile* m_tile = nullptr;
	OpenNode* m_parent = nullptr;
	float g_local = 1.f;
	float g_total = 0.f;
	float h_estimatedDistToGoal = 0.f;
	float f_score = 0.f;
};
//////////////////////////////////////////////////////////////////////////
class PathGenerator
{
public:
	PathGenerator(Map& map, const IntVector2& start, const IntVector2& end, Pawn* gCostRefPawn);
	~PathGenerator() {}
	
	void OpenPathNode(const IntVector2& tileCoords, OpenNode* parent);

	OpenNode* GetAndCloseBestNode();
	bool Step();
	void BuildFinalPath(OpenNode goal);
	void OpenNeighborIfValid(OpenNode* currentNode, const IntVector2& tileCoords);
public:
	IntVector2 m_start;
	IntVector2 m_end;
	Map& mo_map;
	Pawn* mo_gCostRefPawn = nullptr;
	std::vector<OpenNode*> m_openNodes;
	std::vector<OpenNode*> m_closedNodes;
	Path m_finalPath;
	int m_pathID;

private:
	OpenNode* CreateAnOpenNode(const IntVector2& tileCoords, OpenNode* parent);
};