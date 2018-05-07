#include "Game/World.hpp"

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/Pawn.hpp"
#include "Game/Controller.hpp"
//////////////////////////////////////////////////////////////////////////
World::~World()
{
	ClearMaps();
}

void World::CreateWorld(const std::string& adventureName)
{
	AdventureDefinition* def = AdventureDefinition::getDefinition(adventureName);
	if (def == nullptr)
		return;

	g_spriteBitmap = def->m_sprite;
	g_bitmapFont = def->m_font;

	for (uint i = 0; i < def->m_maps.size(); i++)
	{
		Map* newMap = new Map(MapDefinition::getDefinition(def->m_maps[i]->m_mapDef));
		AddMap(newMap, def->m_maps[i]->m_mapName);
		
		for (uint k = 0; k< def->m_maps[i]->m_features.size(); k++)
		{
			newMap->SpawnFeatureWithHelper(def->m_maps[i]->m_features[k]);
		}
		for (uint k = 0; k < def->m_maps[i]->m_pawns.size(); k++)
		{
			newMap->SpawnPawnWithHelper(def->m_maps[i]->m_pawns[k]);
		}
		for (uint k = 0; k < def->m_maps[i]->m_exits.size(); k++)
		{
			newMap->SpawnFeatureWithHelper(def->m_maps[i]->m_exits[k]->feature, def->m_maps[i]->m_exits[k]->exitPath);
		}
		newMap->m_spawnHelp = def->m_maps[i]->m_entrance;
	}

	LoadMap(def->m_startMapName);
}

//////////////////////////////////////////////////////////////////////////
void World::Update(float deltaSeconds)
{
	mo_currentMap->Update(deltaSeconds);
    mo_currentMap->Act();
}

void World::Render() const
{
    mo_currentMap->Render();
}

void World::DebugRender() const
{
	mo_currentMap->DebugRender();
}

void World::Redraw()
{
	mo_currentMap->Redraw();
}

//////////////////////////////////////////////////////////////////////////
void World::AddMap(Map* newMap, const std::string& mapName)
{
	m_maps[mapName] = newMap;
	newMap->m_name = mapName;
}

void World::LoadMap(const std::string& mapName, bool redraw)
{
	auto found = m_maps.find(mapName);
	if (found != m_maps.end())
	{
		mo_currentMap = found->second;
		if(redraw)
			g_theGame->Redraw();
		
		PlayMusic(mo_currentMap->m_bgm);
	}
}

void World::ClearMaps()
{
	for (std::map<std::string, Map*>::iterator iterator = m_maps.begin(); iterator != m_maps.end(); iterator++)
	{
		SAFE_DELETE(iterator->second);
	}
	m_maps.clear();
}

void World::TransportPawnToMap(Pawn* pawnToTransport, const std::string& mapName)
{
	auto found = m_maps.find(mapName);
	if (found != m_maps.end())
	{
		found->second->TransportInPawn(pawnToTransport);
		if (pawnToTransport->mo_controller->m_controllerId == 0)
		{
			LoadMap(mapName);
		}
	}
}

////////////////////////////////////////////////////////////////////////// COMMANDS //////////////////////////////////////////////////////////////////////////
COMMAND(load_map, "Name the map to load.")
{
	g_theGame->m_theWorld->LoadMap(arguments);
}

COMMAND(add_map, "Name the map definition to add to world.")
{
	static uint numCommandMaps = 0;
	g_theGame->m_theWorld->AddMap(new Map(MapDefinition::getDefinition(arguments)), Stringf("cmd_%s_%i", arguments.c_str(), numCommandMaps));
	++numCommandMaps;
}

COMMAND(map, "Name the map definition to add to world and load it as current map.")
{
	static uint numCommandMaps = 0;
	std::string mapName = Stringf("%s_%i", arguments.c_str(), numCommandMaps);
	g_theGame->m_theWorld->AddMap(new Map(MapDefinition::getDefinition(arguments)), mapName);
	++numCommandMaps;
	g_theGame->m_theWorld->LoadMap(mapName);
}

COMMAND(list_maps, "Lists currently available map names.")
{
	for (std::map<std::string, Map*>::iterator iterator = g_theGame->m_theWorld->m_maps.begin(); iterator != g_theGame->m_theWorld->m_maps.end(); iterator++)
	{
		g_theConsole->addMessage(iterator->second->m_name, Rgba(255,255,128,255));
	}
}