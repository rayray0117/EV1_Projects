#include "Game/PathGenerator.hpp"
//////////////////////////////////////////////////////////////////////////
PathGenerator::PathGenerator(Map& map, const IntVector2& start, const IntVector2& end, Pawn* gCostRefPawn)
	: mo_map(map)
	, m_start(start)
	, m_end(end)
	, mo_gCostRefPawn(gCostRefPawn)
{
	static int nextPathID = 1;
	m_pathID = nextPathID++;
	OpenPathNode(start, nullptr);
}

void PathGenerator::OpenPathNode(const IntVector2& tileCoords, OpenNode* parent)
{
	OpenNode* newOpenNode = CreateAnOpenNode(tileCoords, parent);
	m_openNodes.push_back(newOpenNode);
}

OpenNode* PathGenerator::GetAndCloseBestNode()
{
	float bestFScore = MAX_FLOAT;
	OpenNode* bestNode = nullptr;
	int bestIndex = 0;
	for (int openIndex = 0; openIndex < (int)m_openNodes.size(); openIndex++)
	{
		OpenNode* openNode = m_openNodes[openIndex];
		if (openNode->f_score < bestFScore)
		{
			bestFScore = openNode->f_score;
			bestNode = openNode;
			bestIndex = openIndex;
		}
	}

	m_openNodes[bestIndex] = m_openNodes.back();
	m_openNodes.pop_back();

	bestNode->m_tile->m_isClosedInPathID = m_pathID;
	m_closedNodes.push_back(bestNode);

	return bestNode;
}

bool PathGenerator::Step()
{
	OpenNode* selectedNode = GetAndCloseBestNode();
	IntVector2 selectedTileCoords = selectedNode->m_tile->m_tileCoords;

	if (selectedTileCoords == m_end)
	{
		BuildFinalPath(*selectedNode);
		return true;
	}

	OpenNeighborIfValid(selectedNode, selectedTileCoords + IntVector2::XAXIS); //Right
	OpenNeighborIfValid(selectedNode, selectedTileCoords - IntVector2::XAXIS); //Left
	OpenNeighborIfValid(selectedNode, selectedTileCoords + IntVector2::YAXIS); //Up
	OpenNeighborIfValid(selectedNode, selectedTileCoords - IntVector2::YAXIS); //Down

	return false;
}

void PathGenerator::BuildFinalPath(OpenNode goal)
{
	OpenNode* currentNode = &goal;
	while (currentNode)
	{
		m_finalPath.push_back(currentNode->m_tile);
		currentNode = currentNode->m_parent;
	}
}

void PathGenerator::OpenNeighborIfValid(OpenNode* currentNode, const IntVector2& tileCoords)
{
	Tile* neighborTile = mo_map.getTile(tileCoords);

	if (!neighborTile)
		return; //Was out of bounds
	if (neighborTile->m_isClosedInPathID == m_pathID)
		return; //Was a closed node
	if (neighborTile->isSolid())
		return; //Tile is solid


	if (neighborTile->m_isOpenInPathID == m_pathID)
	{
		//Do special open node stuff here!
		return;
	}

	OpenPathNode(tileCoords, currentNode);
}

OpenNode* PathGenerator::CreateAnOpenNode(const IntVector2& tileCoords, OpenNode* parent)
{
	float parentGTotal = parent ? parent->g_total : 0.f;
	OpenNode* newOpenNode = new OpenNode();
	newOpenNode->m_tile = mo_map.getTile(tileCoords);
	newOpenNode->m_parent = parent;
	newOpenNode->h_estimatedDistToGoal = (float)mo_map.CalcManhattanDist(tileCoords, m_end);
	newOpenNode->g_local = newOpenNode->m_tile->getBaseGCost();
	newOpenNode->g_total = parentGTotal + newOpenNode->g_local;
	newOpenNode->f_score = newOpenNode->g_total + newOpenNode->h_estimatedDistToGoal;
	newOpenNode->m_tile->m_isOpenInPathID = m_pathID;
	return newOpenNode;
}

