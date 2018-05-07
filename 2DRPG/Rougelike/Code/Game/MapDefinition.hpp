#pragma once
#include "Engine/General/DataDrivenDefinition.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <vector>
#include "Game/SpawnHelpers.hpp"
////////////////////////////////////////////////////////////////////////// MapDefinition stuff //////////////////////////////////////////////////////////////////////////
class MapGenerator;
//////////////////////////////////////////////////////////////////////////
class MapDefinition : public DataDrivenDefinition<MapDefinition>
{
public:
	MapDefinition(const XMLNode& element);
	~MapDefinition();
	
public:
	IntVector2 m_dimensions;
	std::string m_defaultTileName;
	std::vector<MapGenerator*> m_generators;
	std::string m_bgm;
	int m_sightBonus = 0;
	std::vector<SpawnPawnHelper*> m_pawnsToSpawn;
	std::vector<SpawnFeatureHelper*> m_featuresToSpawn;
private:
	void createGeneratorFromNode(const XMLNode& node);
};