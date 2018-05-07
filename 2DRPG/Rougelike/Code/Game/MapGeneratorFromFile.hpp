#pragma once
#include "Game/MapGenerator.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <string>
#include <map>
#include <vector>
//////////////////////////////////////////////////////////////////////////
class MapGeneratorFromFile : public MapGenerator
{
public:
	MapGeneratorFromFile(const XMLNode& element);
	~MapGeneratorFromFile() {};
	
	void Generate(Map* mapToEdit);
public:
	std::string m_filePath;
	std::map<char, std::string> m_legend;
	std::vector<std::string> m_rows;

	IntVector2 m_offset;
	bool m_reversed = false;
private:
	const std::string getTileTypeFromLegendKey(char key) const;
};