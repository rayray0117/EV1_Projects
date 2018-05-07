#pragma once
#include "Engine/Math/IntVector2.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
struct XMLNode;
//////////////////////////////////////////////////////////////////////////
class SpawnAtHelp
{
public:
	SpawnAtHelp(const XMLNode& element);

public:
	IntVector2 m_tileCoords;
	bool m_force = false;
};

class SpawnOnHelp
{
public:
	SpawnOnHelp(const XMLNode& element);

public:
	IntVector2 m_numberRangeToSpawn = IntVector2(0, 0);
	std::string m_tileType;
	int m_triesBeforeQuiting = 10;
};

class SpawnInHelp
{
public:
	SpawnInHelp(const XMLNode& element);

public:
	IntVector2 m_numberRangeToSpawn = IntVector2(0, 0);
	std::string m_tileFaction;
	int m_triesBeforeQuiting = 10;
};

struct DefAndPercent
{
	DefAndPercent(const XMLNode& element);

	float percentChanceToBeAdded = 1.f;
	std::string definitionName;
};

class SpawnInvetoryHelp
{
public:
	SpawnInvetoryHelp(const XMLNode& element);
public:
	std::vector<DefAndPercent> m_loots;
	std::vector<DefAndPercent> m_items;
};
class SpawnEquipmentHelp
{
public:
	SpawnEquipmentHelp(const XMLNode& element);
public:
	bool m_auto = false;
	/*std::vector<DefAndPercent> m_items;*/
};
//-----------------------
class SpawnPawnHelper
{
public:
	SpawnPawnHelper(const XMLNode& element);
	~SpawnPawnHelper() { SAFE_DELETE(m_invetoryToSpawn); SAFE_DELETE(m_equipmentToSpawn); }
public:
	std::string m_pawnType;
	int m_controllerId = 1;
	std::vector<SpawnAtHelp> m_spawnAts;
	std::vector<SpawnOnHelp> m_spawnOns;
	std::vector<SpawnInHelp> m_spawnIns;
	SpawnInvetoryHelp* m_invetoryToSpawn;
	SpawnEquipmentHelp* m_equipmentToSpawn;
};
class SpawnFeatureHelper
{
public:
	SpawnFeatureHelper(const XMLNode& element);
	~SpawnFeatureHelper() { SAFE_DELETE(m_invetoryToSpawn); }

public:
	std::string m_featureType;
	std::vector<SpawnAtHelp> m_spawnAts;
	std::vector<SpawnOnHelp> m_spawnOns;
	std::vector<SpawnInHelp> m_spawnIns;
	SpawnInvetoryHelp* m_invetoryToSpawn;
};

class SpawnNearPawnHelp
{
public:
	SpawnNearPawnHelp(const XMLNode& element);
public:
	std::string m_pawnType;
};
class SpawnNearFactionHelp
{
public:
	SpawnNearFactionHelp(const XMLNode& element);
public:
	std::string m_factionName;
};
class SpawnHelper
{
public:
	SpawnHelper(const XMLNode& element);
	~SpawnHelper();
public:
	std::vector<SpawnNearPawnHelp*> m_nearP;
	std::vector<SpawnNearFactionHelp*> m_nearF;
	std::vector<SpawnAtHelp*> m_at;
	std::vector<SpawnOnHelp*> m_on;
	std::vector<SpawnInHelp*> m_in;
};