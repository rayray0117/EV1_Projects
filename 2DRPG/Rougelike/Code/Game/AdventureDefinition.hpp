#pragma once
#include "Engine/General/DataDrivenDefinition.hpp"
#include <vector>
#include "Game/SpawnHelpers.hpp"
//////////////////////////////////////////////////////////////////////////
class ExitSpawnHelper
{
public:
	ExitSpawnHelper(const XMLNode& element);

public:
	SpawnFeatureHelper* feature;
	std::string exitPath;
};
class MapCreationHelper
{
public:
	MapCreationHelper(const XMLNode& element);
public:
	std::string m_mapName;
	std::string m_mapDef;
	std::vector<ExitSpawnHelper*> m_exits;
	std::vector<SpawnPawnHelper*> m_pawns;
	std::vector<SpawnFeatureHelper*> m_features;
	SpawnHelper* m_entrance = nullptr;
};
//////////////////////////////////////////////////////////////////////////
class AdventureDefinition : public DataDrivenDefinition<AdventureDefinition>
{
public:
	AdventureDefinition(const XMLNode& element);
	~AdventureDefinition() {}
	
public:
	std::string m_title;
	std::string m_font;
	std::string m_sprite;
	std::string m_startMapName;
	std::vector<MapCreationHelper*> m_maps;
};