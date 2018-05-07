#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/EngineBase.hpp"
#include <vector>
#include "Game/Tile.hpp"
#include "Game/SpawnHelpers.hpp"
//////////////////////////////////////////////////////////////////////////
typedef std::vector<Tile*> Path;
//////////////////////////////////////////////////////////////////////////
class Actor;
class MapGenerator;
class MapDefinition;
class PathGenerator;
class SpawnPawnHelper;
class SpawnFeatureHelper;
class SpawnInvetoryHelp;
class SpawnAtHelp;
class SpawnOnHelp;
class SpawnInHelp;
struct XMLNode;
struct RaycastResult
{
	bool didImpact;
	Vector2 impactPosition;
	IntVector2 impactTileCoords;
	float impactFraction;
	Vector2 impactSurfaceNormal;
};
//////////////////////////////////////////////////////////////////////////
class FloatingNumber
{
public:
	FloatingNumber(int number, const Vector2& start, const Vector2& end, float duration, Rgba color = Rgba::RED);

	void Update(float deltaSeconds);
	void Render() const;

	const bool isFinished() const;
public:
	int m_number;
	Vector2 m_startPos;
	Vector2 m_endPos;
	float m_duration;
	float m_timeSinceBorn;
	Rgba m_color;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class Map
{
	friend class World;
public:
	Map() {}
	Map(const MapDefinition* mapDef);
	~Map();

	void Update(float deltaSeconds);
	void Render() const;
	void DebugRender() const;
	void Redraw();
    void Act();
    void NextPawn();//Tell the next Pawn to Act();
	void CleanDeadActors();


    void addPawn(Pawn* pawn); //Mostly just for testing out core functionality. Remove when actually map building is in. Or I may decide I want this and the pawn list.
	void addFeature(Feature* pawn); //Mostly just for testing out core functionality. Remove when actually map building is in. Or I may decide I want this and the feature list.

	Pawn*	 SpawnPawnAt(const std::string& type, const IntVector2& tileCoords, bool force = false);
	Pawn*	 SpawnPawnOn(const std::string& pawnType, const std::string& tileType, int tries = 10);
	Pawn*	 SpawnPawnIn(const std::string& pawnType, const std::string& tileType, int tires = 10);
	Feature* SpawnFeatureAt(const std::string& type, const IntVector2& tileCoords, bool force = false);
	Feature* SpawnFeatureOn(const std::string& featureType, const std::string& tileType, int tries = 10);
	Feature* SpawnFeatureIn(const std::string& featureType, const std::string& tileFaction, int tires = 10);
	void TransportInPawn(Pawn* pawnToAdd);

    uint getTileIndex(const IntVector2& tileCoords) const;
    IntVector2 getTileCoords(unsigned int tileIndex) const;
    Tile* getTile(const IntVector2& tileCoords);
	const Tile* getTile(const IntVector2& tileCoords) const;
	const Tile* getRandomTile() const; //Something for wander behavior to use for its pathing
	const bool isTileSolid(const IntVector2& tileCoords);
	const bool isInBounds(const IntVector2& tileCoords) const;
	const bool isOutBounds(const IntVector2& tileCoords) const;
	void getAllTilesOfType(const std::string& typeName, std::vector<Tile*>& buffer);
	void getAllTilesOfFaction(const std::string& factionName, std::vector<Tile*>& buffer);

	const Vector2 getCursorMapPos() const;

	void addDamageFloatingNumber(const IntVector2& tileCoords, int damage);

	void GeneratePathForActor(const IntVector2& start, const IntVector2& end, Path& out_pathWhenComplete, Pawn* useGCostFor = nullptr);
	void StartSteppedPathForActor(const IntVector2& start, const IntVector2& end, Pawn* useGCostFor = nullptr);
	bool ContinueSteppedPath(Path& out_pathWhenComplete);

	int CalcManhattanDist(const IntVector2& tileCoords, const IntVector2& m_end) const;

	RaycastResult AmWooRaycast_Solid(const Vector2& startPos, const Vector2& endPos);
	RaycastResult AmWooRaycast_Opaque(const Vector2& startPos, const Vector2& endPos);

	Pawn* FindNearestPawnOfType(const std::string& type, const IntVector2& locationToBeNear, Pawn* useGCostFor = nullptr);
	Pawn* FindNearestPawnNotOfType(const std::string& type, const IntVector2& locationToBeNear, Pawn* useGCostFor = nullptr);
	Pawn* FindNearestPawnOfFaction(const std::string& faction, const IntVector2& locationToBeNear, Pawn* useGCostFor = nullptr);
	Pawn* FindNearestPawnNotOfFaction(const std::string& faction, const IntVector2& locationToBeNear, Pawn* useGCostFor = nullptr);
	Tile* FindNearestTileOfType(const std::string& type, const IntVector2& locationToBeNear, Pawn* useGCostFor = nullptr);
	Tile* FindNearestTileNotOfType(const std::string& type, const IntVector2& locationToBeNear, Pawn* useGCostFor = nullptr);

	int getPathsTileDistance(const IntVector2& start, const IntVector2& end, Pawn* useGCostFor = nullptr);

	void getNeighbors_CrossSingle(const IntVector2& originTile, std::vector<Tile*>& neighbors, int size);
	void getNeighbors_SquareSingle(const IntVector2& originTile, std::vector<Tile*>& neighbors, int size);
	void getNeighbors_Radial(const IntVector2& originTile, std::vector<Tile*>& neighbors, int size);

	void setVisibleTiles();
public:
	IntVector2 m_dimensions;
	std::vector<Tile> m_tiles;
	std::vector<Actor*> m_actors;
    std::vector<Pawn*> mo_pawns;
	std::vector<Feature*> mo_features;
    uint m_nextPawn = 0; //Temporary variable to change who's turn it is. Will be removed when stats are in game.
	PathGenerator* m_currentPath = nullptr;
	std::string m_bgm;
	SpawnHelper* m_spawnHelp = nullptr; //Is for transporting more than spawning.

	int m_sightBonus;
	std::string m_name;
private:
	void Render_CameraView() const;

	void Update_FloatingNumbers(float deltaSeconds);
	void Render_FloatingNumbers()const;
	void CleanUp_FloatingNumbers();

	void SpawnPawnWithHelper(SpawnPawnHelper* helper);
	void SpawnFeatureWithHelper(SpawnFeatureHelper* helper, const std::string exitPath = "");
	Inventory* CreateInventoryWithHelper(SpawnInvetoryHelp* helper);
private:
	std::vector<FloatingNumber*> m_floatingNumbers;
};
//////////////////////////////////////////////////////////////////////////
//Inlines
inline unsigned int Map::getTileIndex(const IntVector2& tileCoords) const
{
    uint index = (tileCoords.y * m_dimensions.x) + tileCoords.x;
    return index;
}

inline IntVector2 Map::getTileCoords(unsigned int tileIndex) const
{
    int y = tileIndex / m_dimensions.x;
    int x = tileIndex % m_dimensions.x;
    return IntVector2(x, y);
}

inline Tile* Map::getTile(const IntVector2& tileCoords)
{
	if (isOutBounds(tileCoords))
		return nullptr;

    uint index = getTileIndex(tileCoords);
    return &m_tiles[index];
}

inline const Tile* Map::getTile(const IntVector2& tileCoords) const
{
	if (isOutBounds(tileCoords))
		return nullptr;

	uint index = getTileIndex(tileCoords);
	return &m_tiles[index];
}

inline const bool Map::isInBounds(const IntVector2& tileCoords) const
{
	if (tileCoords.x < 0 || tileCoords.x >= m_dimensions.x)
		return false;
	if (tileCoords.y < 0 || tileCoords.y >= m_dimensions.y)
		return false;

	return true;
}

inline const bool Map::isOutBounds(const IntVector2& tileCoords) const
{
	return !isInBounds(tileCoords);
}