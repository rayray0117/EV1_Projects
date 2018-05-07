#pragma once
#include <map>
#include <vector>
//////////////////////////////////////////////////////////////////////////
class Map;
class Pawn;
//////////////////////////////////////////////////////////////////////////
class World
{
public:
	World(){}
	~World();

	void CreateWorld(const std::string& adventureName);

	void Update(float deltaSeconds);
	void Render() const;
	void DebugRender() const;
	void Redraw();

	void AddMap(Map* newMap, const std::string& mapName);
	void LoadMap(const std::string& mapName, bool redraw = true);
	void ClearMaps();

	void TransportPawnToMap(Pawn* pawnToTransport, const std::string& mapName);

public:
	std::map<std::string, Map*> m_maps;
	Map* mo_currentMap = nullptr;
};