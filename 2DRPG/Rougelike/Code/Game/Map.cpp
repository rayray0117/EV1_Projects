#include "Game/Map.hpp"

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Actor.hpp"
#include "Game/Pawn.hpp"
#include "Game/Feature.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/MapGenerator.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/Camera2D.hpp"
#include "Game/PathGenerator.hpp"
#include "Game/Controller.hpp"
#include "Game/Inventory.hpp"
#include "Game/Item.hpp"
#include "Game/ItemDefinition.hpp"
////////////////////////////////////////////////////////////////////////// FloatingNumbers //////////////////////////////////////////////////////////////////////////
FloatingNumber::FloatingNumber(int number,const Vector2& start, const Vector2& end, float duration, Rgba color /*= Rgba::RED*/)
	: m_number(number)
	, m_startPos(start)
	, m_endPos(end)
	, m_duration(duration)
	, m_color(color)
	, m_timeSinceBorn(0)
{
}
//////////////////////////////////////////////////////////////////////////
void FloatingNumber::Update(float deltaSeconds)
{
	m_timeSinceBorn += deltaSeconds;
}

void FloatingNumber::Render() const
{
	float fractionToEnd = m_timeSinceBorn * inverse_f(m_duration);
	Vector2 position = interpolate(m_startPos, m_endPos, fractionToEnd);
	Rgba color = interpolate(m_color, Rgba(m_color, FLOATING_NUMBER_END_ALPHA), fractionToEnd);

	g_renderer->drawText2DWithBitmapFont_Centered(position, Stringf("%i", m_number), FLOATING_NUMBER_TEXT_HEIGHT, color, FLOATING_NUMBER_TEXT_ASPECT_RATIO, g_renderer->CreateOrGetBitmapFont(g_bitmapFont));
}

const bool FloatingNumber::isFinished() const
{
	return m_timeSinceBorn >= m_duration;
}

////////////////////////////////////////////////////////////////////////// Map //////////////////////////////////////////////////////////////////////////
Map::Map(const MapDefinition* mapDef)
	:Map()
{
	m_dimensions = mapDef->m_dimensions;
	m_tiles.resize(m_dimensions.x * m_dimensions.y); //Resize so some are constructed and we can just change them.

	for (uint i = 0; i < m_tiles.size(); i++)
	{
		m_tiles[i].m_tileCoords = getTileCoords(i);
		m_tiles[i].changeType(mapDef->m_defaultTileName);
	}

	for (MapGenerator* current : mapDef->m_generators)
	{
		current->Generate(this);
	}

	m_bgm = mapDef->m_bgm;
	m_sightBonus = mapDef->m_sightBonus;

	for (uint i = 0; i < mapDef->m_pawnsToSpawn.size(); i++)
	{
		SpawnPawnWithHelper(mapDef->m_pawnsToSpawn[i]);
	}
	for (uint i = 0; i < mapDef->m_featuresToSpawn.size(); i++)
	{
		SpawnFeatureWithHelper(mapDef->m_featuresToSpawn[i]);
	}
}

Map::~Map()
{
	CleanDeadActors();
	for (Actor* currentActor : m_actors)
	{
		SAFE_DELETE(currentActor);
	}
	m_actors.clear();
}
//////////////////////////////////////////////////////////////////////////
void Map::Update(float deltaSeconds)
{
	for (Actor* currentActor : m_actors)
	{
		if(currentActor != nullptr)
			currentActor->Update(deltaSeconds);
	}

	Update_FloatingNumbers(deltaSeconds);
	CleanUp_FloatingNumbers();
}

void Map::Render() const
{
    // Tiles will render everything on them.
	if (g_theGame->m_theCamera->m_viewEntireMap)
	{
		/*/
		for (uint i = 0; i < m_tiles.size(); i++)
		{
			m_tiles[i].Render();
		}
		//*/
		g_renderer->BindTexture("White");
		g_renderer->drawMesh(g_theGame->m_fillMesh);
		BitmapFont* spriteSheet = g_renderer->CreateOrGetBitmapFont(g_spriteBitmap);
		g_renderer->BindTexture(&spriteSheet->getTexture2D());
		g_renderer->drawMesh(g_theGame->m_glyphMesh);
		g_renderer->BindTexture("White");
		g_renderer->drawMesh(g_theGame->m_overlayMesh);
		//*/
	}
	else
	{
		Render_CameraView();
	}

	Render_FloatingNumbers();
}

void Map::DebugRender() const
{
	IntVector2 mapPos = IntVector2(getCursorMapPos());

	g_renderer->drawThickAABB2(mapPos, mapPos + IntVector2::ONE, .125f, Rgba::BLUE, Rgba(0,0,0,0));
	g_renderer->drawDisc(mapPos, .15f, Rgba::BLACK, 4.f);
	g_renderer->drawDisc(mapPos+IntVector2::ONE, .15f, Rgba::BLACK, 4.f);
	g_renderer->drawDisc(mapPos + IntVector2::XAXIS, .15f, Rgba::BLACK, 4.f);
	g_renderer->drawDisc(mapPos + IntVector2::YAXIS, .15f, Rgba::BLACK, 4.f);


	g_renderer->SetOrthoProjection(Vector2::ZERO, Vector2((float)g_windowWidth, (float)g_windowHeight));

	float halfWindowWidth = (float)g_windowWidth / 2.f;
	float leftXPos = halfWindowWidth;
	if (mapPos.x >= m_dimensions.x*.5)
		leftXPos = 0.f;

	const Tile* selectedTile = getTile(mapPos);
	if (!selectedTile)
		return;

	g_renderer->drawAABB2(Vector2(leftXPos, 0.f), Vector2(leftXPos + halfWindowWidth, (float)g_windowHeight), Rgba(0, 0, 0, 32));

	g_renderer->pushMatrix();
	g_renderer->translate2D(leftXPos, (float)g_windowHeight);
	selectedTile->DebugRender();
	g_renderer->popMatrix();
}

void Map::Redraw()
{
	for (uint i = 0; i < m_tiles.size(); i++)
	{
		m_tiles[i].Redraw();
	}
}

void Map::Act()
{
	if (mo_pawns.size() == 0)
		return; //Do nothing there are no actors loaded in the map!


	CleanDeadActors();

	if(mo_pawns[m_nextPawn])
		mo_pawns[m_nextPawn]->Act();
	if (mo_pawns[m_nextPawn] == nullptr)
		NextPawn();
}

void Map::NextPawn()
{
    m_nextPawn++;
    if (m_nextPawn >= mo_pawns.size())
        m_nextPawn = 0;

	g_theGame->Redraw();
}

void Map::CleanDeadActors()
{
	for (uint i = 0; i < mo_pawns.size(); i++)
	{
		if(mo_pawns[i]==nullptr)
			continue;

		if (mo_pawns[i]->isDead())
		{
			if (m_actors[i]->mo_currentTile)
				m_actors[i]->mo_currentTile->mo_pawn = nullptr;
			mo_pawns[i] = nullptr;
		}
		else if (mo_pawns[i]->mo_currentMap != this)
		{
			mo_pawns[i] = nullptr;
		}
	}
	for (uint i = 0; i < mo_features.size(); i++)
	{
		if(mo_features[i] == nullptr)
			continue;
		if (mo_features[i]->isDead())
		{
			if (m_actors[i]->mo_currentTile)
				m_actors[i]->mo_currentTile->mo_feature = nullptr;
			mo_features[i] = nullptr;
		}
	}
	for (uint i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i] == nullptr)
			continue;
		if (m_actors[i]->isDead())
		{
			SAFE_DELETE(m_actors[i]);
		}
		else if(m_actors[i]->mo_currentMap != this)
		{
			m_actors[i] = nullptr;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void Map::addPawn(Pawn* pawn)
{
	pawn->mo_currentMap = this;
    m_actors.push_back(pawn);
    mo_pawns.push_back(pawn);
}

void Map::addFeature(Feature* feature)
{
	feature->mo_currentMap = this;
	m_actors.push_back(feature);
	mo_features.push_back(feature);
}

Pawn* Map::SpawnPawnAt(const std::string& type, const IntVector2& tileCoords, bool force)
{
	Tile* tile = getTile(tileCoords);

	if (tile == nullptr)
		return nullptr;

	if (!tile->isOpen() && !force) //If the space is solid or taken by another Pawn and we aren't forcing the Pawn there
		return nullptr;

	SAFE_DELETE(tile->mo_pawn);
	Pawn* newPawn = new Pawn(type);
	addPawn(newPawn);
	newPawn->mo_currentTile = tile;
	tile->mo_pawn = newPawn;

	return newPawn;
}

Pawn* Map::SpawnPawnOn(const std::string& pawnType, const std::string& tileType, int tries /*= 10*/)
{
	std::vector<Tile*> tilesOfType;
	getAllTilesOfType(tileType, tilesOfType);

	Pawn* newPawn = nullptr;

	for (int i = 0; i < tries; i++)
	{
		if(tilesOfType.size() == 0)
			break;
		int tileToTryIndex = getRandomIntLessThan((int)tilesOfType.size());

		Tile* tile = tilesOfType[tileToTryIndex];
		if (tile == nullptr)
			continue;

		if (!tile->isOpen()) //If the space is solid or taken by another Pawn
			continue;

		SAFE_DELETE(tile->mo_pawn);
		newPawn = new Pawn(pawnType);
		addPawn(newPawn);
		newPawn->mo_currentTile = tile;
		tile->mo_pawn = newPawn;
		break;
	}
 
	return newPawn;
}

Pawn* Map::SpawnPawnIn(const std::string& pawnType, const std::string& tileType, int tries /*= 10*/)
{
	std::vector<Tile*> tilesOfFaction;
	getAllTilesOfFaction(tileType, tilesOfFaction);

	Pawn* newPawn = nullptr;

	for (int i = 0; i < tries; i++)
	{
		int tileToTryIndex = getRandomIntLessThan((int)tilesOfFaction.size());

		Tile* tile = tilesOfFaction[tileToTryIndex];
		if (tile == nullptr)
			 continue;

		if (!tile->isOpen()) //If the space is solid or taken by another Pawn
			 continue;

		SAFE_DELETE(tile->mo_pawn);
		newPawn = new Pawn(pawnType);
		addPawn(newPawn);
		newPawn->mo_currentTile = tile;
		tile->mo_pawn = newPawn;
		break;
	}

	return newPawn;
}

Feature* Map::SpawnFeatureAt(const std::string& type, const IntVector2& tileCoords, bool force /*= false*/)
{
	Tile* tile = getTile(tileCoords);

	if (tile == nullptr)
		return nullptr;

	if (!tile->isOpen() && !force) //If the space is solid or taken by another Pawn and we aren't forcing the Pawn there
		return nullptr;

	SAFE_DELETE(tile->mo_feature);
	Feature* newPawn = new Feature(type);
	addFeature(newPawn);
	newPawn->mo_currentTile = tile;
	tile->mo_feature = newPawn;

	return newPawn;
}

Feature* Map::SpawnFeatureOn(const std::string& featureType, const std::string& tileType, int tries /*= 10*/)
{
	std::vector<Tile*> tilesOfType;
	getAllTilesOfType(tileType, tilesOfType);

	Feature* newPawn = nullptr;

	for (int i = 0; i < tries; i++)
	{
		int tileToTryIndex = getRandomIntLessThan((int)tilesOfType.size());

		Tile* tile = tilesOfType[tileToTryIndex];
		if (tile == nullptr)
			continue;

		if (!tile->isOpen()) //If the space is solid or taken by another Pawn
			continue;

		SAFE_DELETE(tile->mo_feature);
		newPawn = new Feature(featureType);
		addFeature(newPawn);
		newPawn->mo_currentTile = tile;
		tile->mo_feature = newPawn;
		break;
	}

	return newPawn;
}

Feature* Map::SpawnFeatureIn(const std::string& featureType, const std::string& tileFaction, int tries /*= 10*/)
{
	std::vector<Tile*> tilesOfFaction;
	getAllTilesOfFaction(tileFaction, tilesOfFaction);

	Feature* newPawn = nullptr;

	for (int i = 0; i < tries; i++)
	{
		int tileToTryIndex = getRandomIntLessThan((int)tilesOfFaction.size());

		Tile* tile = tilesOfFaction[tileToTryIndex];
		if (tile == nullptr)
			continue;

		if (!tile->isOpen()) //If the space is solid or taken by another Pawn
			continue;

		SAFE_DELETE(tile->mo_feature);
		newPawn = new Feature(featureType);
		addFeature(newPawn);
		newPawn->mo_currentTile = tile;
		tile->mo_feature = newPawn;
		break;
	}

	return newPawn;
}

void Map::TransportInPawn(Pawn* pawnToAdd)
{
	addPawn(pawnToAdd);

	if (!m_spawnHelp)
	{
		const Tile* tileToTry = getRandomTile();
		while (!pawnToAdd->MoveToTile(tileToTry->m_tileCoords));
		{
			tileToTry = getRandomTile();
			
		} 
		return;
	}

	TODO("Finish spawn nears!")
	/*/ //Need to add distance and then get neighbors stuff for this to work
	for (uint i = 0; i < m_spawnHelp->m_nearP.size(); i++)
	{
		if (pawnToAdd->MoveToTile(m_spawnHelp->m_nearP[i]->m_tileCoords))
			return;
	}
	for (uint i = 0; i < m_spawnHelp->m_nearF.size(); i++)
	{
		if (pawnToAdd->MoveToTile(m_spawnHelp->m_nearF[i]->m_tileCoords))
			return;
	}
	//*/

	for (uint i = 0; i < m_spawnHelp->m_at.size(); i++)
	{
		if (pawnToAdd->MoveToTile(m_spawnHelp->m_at[i]->m_tileCoords))
			return;
	}

	for (uint i = 0; i < m_spawnHelp->m_on.size(); i++)
	{
		std::vector<Tile*> tilesOfType;
		getAllTilesOfType(m_spawnHelp->m_on[i]->m_tileType, tilesOfType);

		for (int k = 0; k <  m_spawnHelp->m_on[i]->m_triesBeforeQuiting; k++)
		{
			if (tilesOfType.size() == 0)
				break;
			int tileToTryIndex = getRandomIntLessThan((int)tilesOfType.size());

			Tile* tile = tilesOfType[tileToTryIndex];
			if (pawnToAdd->MoveToTile(tile))
				return;
		}
	}
	for (uint i = 0; i < m_spawnHelp->m_in.size(); i++)
	{
		std::vector<Tile*> tilesOfFaction;
		getAllTilesOfFaction(m_spawnHelp->m_in[i]->m_tileFaction, tilesOfFaction);

		for (int k = 0; k < m_spawnHelp->m_in[i]->m_triesBeforeQuiting; k++)
		{
			int tileToTryIndex = getRandomIntLessThan((int)tilesOfFaction.size());

			Tile* tile = tilesOfFaction[tileToTryIndex];
			if (pawnToAdd->MoveToTile(tile))
				return;
		}
	}

	const Tile* tileToTry;
	do
	{
		tileToTry = getRandomTile();
		pawnToAdd->MoveToTile(tileToTry->m_tileCoords);
	} while (!tileToTry->isOpen());
}

//////////////////////////////////////////////////////////////////////////
const Tile* Map::getRandomTile() const
{
	int tileIndex = getRandomIntLessThan((int)m_tiles.size());
	return &m_tiles[tileIndex];
}

const bool Map::isTileSolid(const IntVector2& tileCoords)
{
	Tile* tileToCheck = getTile(tileCoords);
	return tileToCheck->mo_tileDef->m_isSolid;
}

void Map::getAllTilesOfType(const std::string& typeName, std::vector<Tile*>& buffer)
{
	for (uint i = 0; i < m_tiles.size(); i++)
	{
		if (m_tiles[i] == typeName)
			buffer.push_back(&m_tiles[i]);
	}
}

void Map::getAllTilesOfFaction(const std::string& factionName, std::vector<Tile*>& buffer)
{
	for (uint i = 0; i < m_tiles.size(); i++)
	{
		if (m_tiles[i].m_faction == factionName)
			buffer.push_back(&m_tiles[i]);
	}
}

//////////////////////////////////////////////////////////////////////////
const Vector2 Map::getCursorMapPos() const
{
	Vector2 cursorPos = g_myWindow->getCursorNormalizedPos(g_inputSystem);

	Vector2 bottomLeft;
	Vector2 topRight;
	g_theGame->m_theCamera->getOrtho(bottomLeft, topRight);

	Vector2 mapCursorPos;
	mapCursorPos.x = rangeMapFloat(cursorPos.x, 0.f, 1.f, bottomLeft.x, topRight.x);
	mapCursorPos.y = rangeMapFloat(cursorPos.y, 0.f, 1.f, topRight.y, bottomLeft.y);
	return mapCursorPos;
}

void Map::addDamageFloatingNumber(const IntVector2& tileCoords, int damage)
{
	Vector2 startPos(tileCoords);
	startPos += Vector2(.5f, .5f);

	Vector2 endPos = startPos + Vector2(0.f, FLOATING_NUMBER_END_HEIGHT);

	FloatingNumber* f = new FloatingNumber(damage, startPos, endPos, FLOATING_NUMBER_DURATION);
	m_floatingNumbers.push_back(f);
}
//////////////////////////////////////////////////////////////////////////
void Map::GeneratePathForActor(const IntVector2& start, const IntVector2& end, Path& out_pathWhenComplete, Pawn* useGCostFor)
{
	StartSteppedPathForActor(start, end, useGCostFor);
	while (!ContinueSteppedPath(out_pathWhenComplete));
}

void Map::StartSteppedPathForActor(const IntVector2& start, const IntVector2& end, Pawn* useGCostFor)
{
	SAFE_DELETE(m_currentPath);

	m_currentPath = new PathGenerator(*this, start, end, useGCostFor);
}

bool Map::ContinueSteppedPath(Path& out_pathWhenComplete)
{
	GUARANTEE_OR_DIE(m_currentPath != nullptr, "ERROR: Cannot continue a path that doesn't exist!");
	bool isComplete = m_currentPath->Step();
	if (isComplete)
	{
		out_pathWhenComplete = m_currentPath->m_finalPath;
		return true;
	}

	return false;
}

int Map::CalcManhattanDist(const IntVector2& start, const IntVector2& end) const
{
	return abs(start.x - end.x) + abs(start.y - end.y);
}

RaycastResult Map::AmWooRaycast_Solid(const Vector2& startPos, const Vector2& endPos)
{
	RaycastResult result;

	IntVector2 startCoords(startPos);
	Tile* currentTile = getTile(startCoords);


	if (currentTile->isSolid())
	{
		result.didImpact = true;
		result.impactFraction = 0.f;
		result.impactPosition = startPos;
		result.impactTileCoords = startCoords;
		result.impactSurfaceNormal = Vector2::ZERO;
		return result;
	}

	Vector2 rayDisplacement = endPos - startPos;

	//Values
	Vector2 tDelta;
	IntVector2 tileStep;
	Vector2 offsetToLeadingEdge;
	Vector2 tOfNextCrossing;

	//X values
	tDelta.x = inverse_f(abs(rayDisplacement.x));
	tDelta.y = inverse_f(abs(rayDisplacement.y));
	if (rayDisplacement.x > 0)
		tileStep.x = 1;
	else
		tileStep.x = -1;
	if (rayDisplacement.y > 0)
		tileStep.y = 1;
	else
		tileStep.y = -1;


	offsetToLeadingEdge = Vector2(tileStep + IntVector2::ONE) * .5f;

	float firstVerticalIntersectionX = (float)(startCoords.x + offsetToLeadingEdge.x);
	tOfNextCrossing.x = abs(firstVerticalIntersectionX - startPos.x) * tDelta.x;

	//Y values
	float firstVerticalIntersectionY = (float)(startCoords.y + offsetToLeadingEdge.y);
	tOfNextCrossing.y = abs(firstVerticalIntersectionY - startPos.y) * tDelta.y;

	IntVector2 tilePos = startCoords;
	while (true)
	{
		if (tOfNextCrossing.x < tOfNextCrossing.y)
		{
			if (tOfNextCrossing.x > 1)
			{
				// No impact
				result.didImpact = false;
				return result;
			}

			tilePos.x += tileStep.x;

			Tile* tileToCheck = getTile(tilePos);
			if (tileToCheck && tileToCheck->isSolid())
			{
				//Impact
				result.didImpact = true;
				result.impactFraction = tOfNextCrossing.x;
				result.impactPosition = startPos + (rayDisplacement * tOfNextCrossing);
				result.impactTileCoords = tilePos;
				result.impactSurfaceNormal = Vector2(-(float)tileStep.x, 0.f);
				return result;
			}
			
			tOfNextCrossing.x += tDelta.x;
		}
		else if(tOfNextCrossing.y < tOfNextCrossing.x)
		{
			if (tOfNextCrossing.y > 1)
			{
				// No impact
				result.didImpact = false;
				return result;
			}

			tilePos.y += tileStep.y;

			Tile* tileToCheck = getTile(tilePos);
			if (tileToCheck && tileToCheck->isSolid())
			{
				//Impact
				result.didImpact = true;
				result.impactFraction = tOfNextCrossing.y;
				result.impactPosition = startPos + (rayDisplacement * tOfNextCrossing);
				result.impactTileCoords = tilePos;
				result.impactSurfaceNormal = Vector2(0.f, -(float)tileStep.y);
				return result;
			}

			tOfNextCrossing.y += tDelta.y;
		}
		else
		{
			if (tOfNextCrossing.y > 1 || tOfNextCrossing.x > 1)
			{
				// No impact
				result.didImpact = false;
				return result;
			}

			tilePos += tileStep;

			Tile* tileTemp = getTile(tilePos);
			if (tileTemp && tileTemp->isSolid())
			{
				//Impact
				result.didImpact = true;
				result.impactFraction = tOfNextCrossing.y;
				result.impactPosition = startPos + (rayDisplacement * tOfNextCrossing);
				result.impactTileCoords = tilePos;
				result.impactSurfaceNormal = -Vector2(tileStep);
				return result;
			}

			tOfNextCrossing += tDelta;
		}
	}
}

RaycastResult Map::AmWooRaycast_Opaque(const Vector2& startPos, const Vector2& endPos)
{
	RaycastResult result;

	IntVector2 startCoords(startPos);
	Tile* currentTile = getTile(startCoords);


	if (currentTile->isOpaque())
	{
		result.didImpact = true;
		result.impactFraction = 0.f;
		result.impactPosition = startPos;
		result.impactTileCoords = startCoords;
		result.impactSurfaceNormal = Vector2::ZERO;
		return result;
	}

	Vector2 rayDisplacement = endPos - startPos;

	//Values
	Vector2 tDelta(1.f,1.f);
	IntVector2 tileStep;
	Vector2 offsetToLeadingEdge;
	Vector2 tOfNextCrossing;

	//X values

	tDelta.x = 1/abs(rayDisplacement.x);
	tDelta.y = 1/abs(rayDisplacement.y);

	if (rayDisplacement.x > 0)
		tileStep.x = 1;
	else
		tileStep.x = -1;
	if (rayDisplacement.y > 0)
		tileStep.y = 1;
	else
		tileStep.y = -1;


	offsetToLeadingEdge = Vector2(tileStep + IntVector2::ONE) * .5f;

	float firstVerticalIntersectionX = (float)(startCoords.x + offsetToLeadingEdge.x);
	tOfNextCrossing.x = abs(firstVerticalIntersectionX - startPos.x) * tDelta.x;

	//Y values
	float firstVerticalIntersectionY = (float)(startCoords.y + offsetToLeadingEdge.y);
	tOfNextCrossing.y = abs(firstVerticalIntersectionY - startPos.y) * tDelta.y;

	IntVector2 tilePos = startCoords;
	while (true)
	{
		if (tOfNextCrossing.x < tOfNextCrossing.y)
		{
			if (tOfNextCrossing.x > 1)
			{
				// No impact
				result.didImpact = false;
				return result;
			}

			tilePos.x += tileStep.x;

			Tile* tileTemp = getTile(tilePos);
			if (tileTemp && tileTemp->isOpaque())
			{
				//Impact
				result.didImpact = true;
				result.impactFraction = tOfNextCrossing.x;
				result.impactPosition = startPos + (rayDisplacement * tOfNextCrossing);
				result.impactTileCoords = tilePos;
				result.impactSurfaceNormal = Vector2(-(float)tileStep.x, 0.f);
				return result;
			}

			tOfNextCrossing.x += tDelta.x;
		}
		else if(tOfNextCrossing.y < tOfNextCrossing.x)
		{
			if (tOfNextCrossing.y > 1)
			{
				// No impact
				result.didImpact = false;
				return result;
			}

			tilePos.y += tileStep.y;

			Tile* tileTemp = getTile(tilePos);
			if (tileTemp && tileTemp->isOpaque())
			{
				//Impact
				result.didImpact = true;
				result.impactFraction = tOfNextCrossing.y;
				result.impactPosition = startPos + (rayDisplacement * tOfNextCrossing);
				result.impactTileCoords = tilePos;
				result.impactSurfaceNormal = Vector2(0.f, -(float)tileStep.y);
				return result;
			}

			tOfNextCrossing.y += tDelta.y;
		}
		else
		{
			if (tOfNextCrossing.y > 1 || tOfNextCrossing.x  > 1)
			{
				// No impact
				result.didImpact = false;
				return result;
			}

			tilePos += tileStep;

			Tile* tileTemp = getTile(tilePos);
			if (tileTemp && tileTemp->isOpaque())
			{
				//Impact
				result.didImpact = true;
				result.impactFraction = tOfNextCrossing.y;
				result.impactPosition = startPos + (rayDisplacement * tOfNextCrossing);
				result.impactTileCoords = tilePos;
				result.impactSurfaceNormal = -Vector2(tileStep);
				return result;
			}

			tOfNextCrossing += tDelta;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
Pawn* Map::FindNearestPawnOfType(const std::string& type, const IntVector2& locationToBeNear, Pawn* useGCostFor)
{
	Pawn* nearestPawn = nullptr;
	int nearestTileDistance = INT_MAX;
	for (Pawn* currentPawn : mo_pawns)
	{
		if(currentPawn == useGCostFor || *currentPawn != type)
			continue;

		
		int tileDistToCurrentPawn = getPathsTileDistance(locationToBeNear, currentPawn->getTileCoords(), useGCostFor);
		if (tileDistToCurrentPawn < nearestTileDistance)
		{
			nearestTileDistance = tileDistToCurrentPawn;
			nearestPawn = currentPawn;
		}
	}

	return nearestPawn;
}

Pawn* Map::FindNearestPawnNotOfType(const std::string& type, const IntVector2& locationToBeNear, Pawn* useGCostFor)
{
	Pawn* nearestPawn = nullptr;
	int nearestTileDistance = INT_MAX;
	for (Pawn* currentPawn : mo_pawns)
	{
		if (currentPawn == useGCostFor || *currentPawn == type)
			continue;


		int tileDistToCurrentPawn = getPathsTileDistance(locationToBeNear, currentPawn->getTileCoords(), useGCostFor);
		if (tileDistToCurrentPawn < nearestTileDistance)
		{
			nearestTileDistance = tileDistToCurrentPawn;
			nearestPawn = currentPawn;
		}
	}

	return nearestPawn;
}

Pawn* Map::FindNearestPawnOfFaction(const std::string& faction, const IntVector2& locationToBeNear, Pawn* useGCostFor)
{
	Pawn* nearestPawn = nullptr;
	int nearestTileDistance = INT_MAX;
	for (Pawn* currentPawn : mo_pawns)
	{
		if (currentPawn == useGCostFor || currentPawn->m_faction != faction)
			continue;


		int tileDistToCurrentPawn = getPathsTileDistance(locationToBeNear, currentPawn->getTileCoords(), useGCostFor);
		if (tileDistToCurrentPawn < nearestTileDistance)
		{
			nearestTileDistance = tileDistToCurrentPawn;
			nearestPawn = currentPawn;
		}
	}

	return nearestPawn;
}

Pawn* Map::FindNearestPawnNotOfFaction(const std::string& faction, const IntVector2& locationToBeNear, Pawn* useGCostFor)
{
	Pawn* nearestPawn = nullptr;
	int nearestTileDistance = INT_MAX;
	for (Pawn* currentPawn : mo_pawns)
	{
		if (currentPawn == useGCostFor || currentPawn->m_faction == faction)
			continue;


		int tileDistToCurrentPawn = getPathsTileDistance(locationToBeNear, currentPawn->getTileCoords(), useGCostFor);
		if (tileDistToCurrentPawn < nearestTileDistance)
		{
			nearestTileDistance = tileDistToCurrentPawn;
			nearestPawn = currentPawn;
		}
	}

	return nearestPawn;
}

Tile* Map::FindNearestTileOfType(const std::string& type, const IntVector2& locationToBeNear, Pawn* useGCostFor)
{
	Tile* nearestTile = nullptr;
	int nearestTileDistance = INT_MAX;
	for (Tile currentTile : m_tiles)
	{
		if (currentTile != type)
			continue;


		int tileDistToCurrentPawn = getPathsTileDistance(locationToBeNear, currentTile.m_tileCoords, useGCostFor);
		if (tileDistToCurrentPawn < nearestTileDistance)
		{
			nearestTileDistance = tileDistToCurrentPawn;
			nearestTile = &currentTile;
		}
	}

	return nearestTile;
}

Tile* Map::FindNearestTileNotOfType(const std::string& type, const IntVector2& locationToBeNear, Pawn* useGCostFor)
{
	Tile* nearestTile = nullptr;
	int nearestTileDistance = INT_MAX;
	for (Tile currentTile : m_tiles)
	{
		if (currentTile == type)
			continue;


		int tileDistToCurrentPawn = getPathsTileDistance(locationToBeNear, currentTile.m_tileCoords, useGCostFor);
		if (tileDistToCurrentPawn < nearestTileDistance)
		{
			nearestTileDistance = tileDistToCurrentPawn;
			nearestTile = &currentTile;
		}
	}

	return nearestTile;
}

int Map::getPathsTileDistance(const IntVector2& start, const IntVector2& end, Pawn* useGCostFor /*= nullptr*/)
{
	Path pathToGoal;
	GeneratePathForActor(start, end, pathToGoal, useGCostFor);
	return (int)pathToGoal.size();
}

void Map::getNeighbors_CrossSingle(const IntVector2& originTile, std::vector<Tile*>& neighbors, int /*size*/)
{
	// Not using size yet because I don't need it till later when you can move more than one space and attack effect more than one space.
	neighbors.push_back(getTile(originTile + IntVector2::XAXIS));
	neighbors.push_back(getTile(originTile - IntVector2::XAXIS));
	neighbors.push_back(getTile(originTile + IntVector2::YAXIS));
	neighbors.push_back(getTile(originTile - IntVector2::YAXIS));
}

void Map::getNeighbors_SquareSingle(const IntVector2& originTile, std::vector<Tile*>& neighbors, int size)
{
	getNeighbors_CrossSingle(originTile, neighbors, size);

	neighbors.push_back(getTile(originTile + IntVector2::ONE));
	neighbors.push_back(getTile(originTile - IntVector2::ONE));
	neighbors.push_back(getTile(originTile + IntVector2(-1,1)));
	neighbors.push_back(getTile(originTile - IntVector2(-1,1)));
}

void Map::getNeighbors_Radial(const IntVector2& originTile, std::vector<Tile*>& neighbors, int size)
{
	for (uint i = 0; i < m_tiles.size(); i++)
	{
		if (CalcManhattanDist(originTile, m_tiles[i].m_tileCoords) <= size)
		{
			neighbors.push_back(&m_tiles[i]);
		}
	}
}

void Map::setVisibleTiles()
{
	//reset all seen tiles to was seen.
	for (uint i = 0; i < m_tiles.size(); i++)
	{
		if (m_tiles[i].m_visibilty == TILE_VIS_IS_SEEN)
			m_tiles[i].m_visibilty = TILE_VIS_WAS_SEEN;
	}
	

	for (Pawn* current : mo_pawns)
	{
		if(!current)
			continue;
		if (current->mo_controller == Controller::s_controllers[0])
		{
			current->setVisibleTiles(m_sightBonus);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void Map::Render_CameraView() const
{
	IntVector2 bottomLeft;
	IntVector2 topRight;
	g_theGame->m_theCamera->getCameraView(bottomLeft, topRight);

	/*/
	for (Tile current : m_tiles) //Check every tile and only render the ones in view.
	{
		if (current.m_tileCoords >= bottomLeft && current.m_tileCoords < topRight)
			current.Render();
	}
	//*/
	g_renderer->BindTexture("White");
	g_renderer->drawMesh(g_theGame->m_fillMesh);
	BitmapFont* spriteSheet = g_renderer->CreateOrGetBitmapFont(g_spriteBitmap);
	g_renderer->BindTexture(&spriteSheet->getTexture2D());
	g_renderer->drawMesh(g_theGame->m_glyphMesh);
	g_renderer->BindTexture("White");
	g_renderer->drawMesh(g_theGame->m_overlayMesh);
	//*/
}
//////////////////////////////////////////////////////////////////////////
void Map::Update_FloatingNumbers(float deltaSeconds)
{
	for (FloatingNumber* current : m_floatingNumbers)
	{
		if(current)
			current->Update(deltaSeconds);
	}
}

void Map::Render_FloatingNumbers() const
{
	for (FloatingNumber* current : m_floatingNumbers)
	{
		if (current)
			current->Render();
	}
}

void Map::CleanUp_FloatingNumbers()
{
	for (uint i = 0; i < m_floatingNumbers.size(); i++)
	{
		if (m_floatingNumbers[i]->isFinished())
		{
			SAFE_DELETE(m_floatingNumbers[i]);
			m_floatingNumbers.erase(m_floatingNumbers.begin() + i);
		}
	}
}

void Map::SpawnPawnWithHelper(SpawnPawnHelper* helper)
{
	std::vector<Pawn*> spawnedPawns;
	for (uint i = 0; i < helper->m_spawnAts.size(); i++)
	{
		Pawn* temp = SpawnPawnAt(helper->m_pawnType, helper->m_spawnAts[i].m_tileCoords, helper->m_spawnAts[i].m_force);
		if (temp)
			spawnedPawns.push_back(temp);
	}
	for (uint i = 0; i < helper->m_spawnOns.size(); i++)
	{
		int numToSpawn = getRandomIntInRange(helper->m_spawnOns[i].m_numberRangeToSpawn.x, helper->m_spawnOns[i].m_numberRangeToSpawn.y);
		for (int count = 0; count < numToSpawn; count++)
		{
			Pawn* temp = SpawnPawnOn(helper->m_pawnType, helper->m_spawnOns[i].m_tileType, helper->m_spawnOns[i].m_triesBeforeQuiting);
			if (temp)
				spawnedPawns.push_back(temp);
		}
	}
	for (uint i = 0; i < helper->m_spawnIns.size(); i++)
	{
		int numToSpawn = getRandomIntInRange(helper->m_spawnIns[i].m_numberRangeToSpawn.x, helper->m_spawnIns[i].m_numberRangeToSpawn.y);
		for (int count = 0; count < numToSpawn; count++)
		{
			Pawn* temp = SpawnPawnIn(helper->m_pawnType, helper->m_spawnIns[i].m_tileFaction, helper->m_spawnIns[i].m_triesBeforeQuiting);
			if (temp)
				spawnedPawns.push_back(temp);
		}
	}

	for (uint i = 0; i < spawnedPawns.size(); i++)
	{
		spawnedPawns[i]->AssignController(helper->m_controllerId);
		spawnedPawns[i]->m_inventory = CreateInventoryWithHelper(helper->m_invetoryToSpawn);

		spawnedPawns[i]->currentHP = spawnedPawns[i]->getCombinedStats().m_stats[STAT_HEALTH];
		spawnedPawns[i]->currentStamina = spawnedPawns[i]->getCombinedStats().m_stats[STAT_STAMINA];
	}

}

void Map::SpawnFeatureWithHelper(SpawnFeatureHelper* helper, const std::string exitPath)
{
	std::vector<Feature*> spawnedFeatures;
	for (uint i = 0; i < helper->m_spawnAts.size(); i++)
	{
		Feature* temp = SpawnFeatureAt(helper->m_featureType, helper->m_spawnAts[i].m_tileCoords, helper->m_spawnAts[i].m_force);
		if (temp)
			spawnedFeatures.push_back(temp);
	}
	for (uint i = 0; i < helper->m_spawnOns.size(); i++)
	{
		int numToSpawn = getRandomIntInRange(helper->m_spawnOns[i].m_numberRangeToSpawn.x, helper->m_spawnOns[i].m_numberRangeToSpawn.y);
		for (int count = 0; count < numToSpawn; count++)
		{
			Feature* temp = SpawnFeatureOn(helper->m_featureType, helper->m_spawnOns[i].m_tileType, helper->m_spawnOns[i].m_triesBeforeQuiting);
			if (temp)
				spawnedFeatures.push_back(temp);
		}
	}
	for (uint i = 0; i < helper->m_spawnIns.size(); i++)
	{
		int numToSpawn = getRandomIntInRange(helper->m_spawnIns[i].m_numberRangeToSpawn.x, helper->m_spawnIns[i].m_numberRangeToSpawn.y);
		for (int count = 0; count < numToSpawn; count++)
		{
			Feature* temp = SpawnFeatureIn(helper->m_featureType, helper->m_spawnIns[i].m_tileFaction, helper->m_spawnIns[i].m_triesBeforeQuiting);
			if (temp)
				spawnedFeatures.push_back(temp);
		}
	}
	for (uint i = 0; i < spawnedFeatures.size(); i++)
	{
		spawnedFeatures[i]->m_inventory = CreateInventoryWithHelper(helper->m_invetoryToSpawn);
		spawnedFeatures[i]->currentHP = spawnedFeatures[i]->m_baseStats.m_stats[STAT_HEALTH];
		if (spawnedFeatures[i]->currentHP == 0)
			spawnedFeatures[i]->currentHP = 1;
		spawnedFeatures[i]->m_exitToMapName = exitPath;
	}
}

Inventory* Map::CreateInventoryWithHelper(SpawnInvetoryHelp* helper)
{
	if (helper == nullptr)
		return nullptr;

	Inventory* inventory = new Inventory();
	for (uint i = 0; i < helper->m_items.size(); i++)
	{
		if (isPercentChance(helper->m_items[i].percentChanceToBeAdded))
		{
			Item* addedItem = new Item(ItemDefinition::getDefinition(helper->m_items[i].definitionName));
			if (addedItem)
				inventory->AddItem(addedItem);
		}
	}

	return inventory;
	TODO("Create and handle loot system for Invertory Spawning!");
}
