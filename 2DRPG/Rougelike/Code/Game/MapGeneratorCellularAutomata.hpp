#pragma once
#include "Game/MapGenerator.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
struct CellRule
{
	CellRule(const XMLNode& element);

	const bool isInRange(int value) const;
	void ChangeIfValidTile(Map* mapToEdit, const IntVector2& tileCoords, std::vector<std::string>& tempMap);

	IntVector2 range;
	std::string changeToType = "";
	std::string ifMyTile = "";
	std::string ifThierTile = "";
	float chanceToRun = 1.f;
};
//////////////////////////////////////////////////////////////////////////
class MapGeneratorCellularAutomata : public MapGenerator
{
public:
	MapGeneratorCellularAutomata(const XMLNode& element);
	~MapGeneratorCellularAutomata() {}
	

	void Generate(Map* mapToEdit) override;
public:
	IntVector2 m_dimensions;
	IntVector2 m_offset;
	std::vector<CellRule> m_rules;
	std::vector<std::string> m_tempMap;
	int m_numIterations = 1;
};