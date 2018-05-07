#include "Game/Pawn.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Controller.hpp"
#include "Game/Tile.hpp"
#include "Game/Map.hpp"
#include "Game/World.hpp"
#include "Game/Game.hpp"
#include "Game/Item.hpp"
#include "Game/Battle.hpp"
#include "Game/Behavior.hpp"
#include "Game/Inventory.hpp"
#include "Game/Feature.hpp"

#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
//////////////////////////////////////////////////////////////////////////
Pawn::Pawn()
	:Actor()
{
	for (int i = 0; i < NUM_EQSLOTS; i++)
	{
		mo_equipment[i] = nullptr;
	}
}

Pawn::Pawn(const std::string pawnDefName)
	:Pawn()
{
	mo_def = PawnDefinition::getDefinition(pawnDefName);
	m_name = mo_def->m_name;
	m_faction = mo_def->m_faction;

	m_baseStats = mo_def->m_initialStats;

	m_sprite = mo_def->m_sprite;

	for each (Behavior* current in mo_def->m_behaviors)
	{
		m_behaviors.push_back(current->Clone());
	}

	m_tookDamageSoundEffect = mo_def->m_tookDamageSound;
}

Pawn::Pawn(const std::string pawnDefName, uint controllerId)
	:Pawn(pawnDefName)
{
	AssignController(controllerId);
}

Pawn::~Pawn()
{
	for (Behavior* current : m_behaviors)
	{
		SAFE_DELETE(current);
	}
	Actor::~Actor();
}
//////////////////////////////////////////////////////////////////////////
void Pawn::DebugRender() const
{
	BitmapFont* f = g_renderer->CreateOrGetBitmapFont(g_bitmapFont);
	g_renderer->DrawText2D(Vector2::ZERO, Stringf("Pawn name: %s, Faction: %s", mo_def->m_name.c_str(), m_faction.c_str()), 25.f, Rgba::WHITE, 1.f, f);
	g_renderer->translate2D(Vector2(0.f, -25.f));
 	
	for (Behavior* current : m_behaviors)
	{
		current->DebugRender();
	}
	if (m_inventory)
		m_inventory->DebugRender();
}

//////////////////////////////////////////////////////////////////////////
void Pawn::Act()
{
	//If my controller isn't already using me make them.
	if(mo_controller->mo_controlledPawn != this)
		mo_controller->Possess(this);
	mo_controller->Act();
}
//////////////////////////////////////////////////////////////////////////
bool Pawn::MoveToTile(Tile* tile)
{
	if (tile == nullptr) //If I really wanted it to go to a null tile I'd just set it's mo_currentTile to null
		return false;
	
	if (mo_currentTile == tile)
		return true;

    if (!tile->isOpen())
        return false;

	if (tile->mo_feature && tile->mo_feature->m_isExit)
	{
		g_theGame->m_theWorld->TransportPawnToMap(this, tile->mo_feature->m_exitToMapName);
	}


    if (mo_currentTile)
    {
        mo_currentTile->mo_pawn = nullptr;
    }

    mo_currentTile = tile;
    mo_currentTile->mo_pawn = this;
    return true;
}

bool Pawn::MoveToTile(const IntVector2& tileCoords)
{
	Map* currentMap = mo_currentMap;
    Tile* tile = currentMap->getTile(tileCoords);
    return MoveToTile(tile);
}

bool Pawn::AttackTile(Tile* tile)
{
	if (!tile || !tile->mo_pawn || tile->mo_pawn->m_faction == m_faction) //Fix this later so it'll work with attack moves when those are added in and not a simple getting it working thing
	{
		/* DO NOTHING */
		return false;
	}

	Battle::BasicFight(*this, *tile); TODO("Replace this with real fight system when implemented!");
	return true;
}

bool Pawn::AttackTile(const IntVector2& tileCoords)
{
	Map* currentMap = mo_currentMap;
	Tile* tile = currentMap->getTile(tileCoords);
	return AttackTile(tile);
}

//////////////////////////////////////////////////////////////////////////
void Pawn::AssignController(uint controllerId)
{
    mo_controller = Controller::s_controllers[controllerId];
}
//////////////////////////////////////////////////////////////////////////
const Stats Pawn::getCombinedStats() const
{
	Stats result = m_baseStats;
	for (uint i = 0; i < NUM_EQSLOTS; i++)
	{
		if(mo_equipment[i])
			result += mo_equipment[i]->mo_stats;
	}
	// May want to make sure a stat isn't negative?
	return result;
}

float Pawn::getCurrentHealthPercent() const
{
	Stats totalStats = getCombinedStats();
	float currentHealthPercent = (float)currentHP / (float)totalStats.m_stats[STAT_HEALTH];
	return currentHealthPercent;
}

void Pawn::FindTarget()
{
	for (Pawn* current : mo_currentMap->mo_pawns)
	{
		if(current == nullptr)
			continue;
		if(current->m_faction == m_faction)
			continue;

		RaycastResult ray = mo_currentMap->AmWooRaycast_Opaque(getTileCoords(), current->getTileCoords());
		if (ray.didImpact)
		{
			mo_target = current; //Just make the first one I find my target
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void Pawn::takeDirectDamage(int damage)
{
	currentHP -= damage;

	g_theConsole->addMessage(Stringf("Attacker did %i damage to %s!", damage, m_name.c_str()), Rgba::RED);
	mo_currentMap->addDamageFloatingNumber(getTileCoords(), damage);

	PlaySFX(m_tookDamageSoundEffect);
}

void Pawn::setVisibleTiles(int mapSightBonus)
{
	mo_currentTile->m_visibilty = TILE_VIS_IS_SEEN;

	int sightDist = mapSightBonus + getCombinedStats().m_stats[STAT_SIGHT];
	
	std::vector<Tile*> tilesInSightRange;
	mo_currentMap->getNeighbors_Radial(getTileCoords(), tilesInSightRange, sightDist);

	for (Tile* current : tilesInSightRange)
	{
		RaycastResult ray = mo_currentMap->AmWooRaycast_Opaque(Vector2(getTileCoords()) + Vector2(.5f, .5f), Vector2(current->m_tileCoords) + Vector2(.5f, .5f));
		if (!ray.didImpact)
			current->m_visibilty = TILE_VIS_IS_SEEN;
		else if(ray.impactTileCoords == current->m_tileCoords)
			current->m_visibilty = TILE_VIS_IS_SEEN;
	}
}

void Pawn::setVisibleActors()
{
	m_visablePawns.clear();
	int sightDist = mo_currentMap->m_sightBonus + getCombinedStats().m_stats[STAT_SIGHT];

	std::vector<Tile*> tilesInSightRange;
	mo_currentMap->getNeighbors_Radial(getTileCoords(), tilesInSightRange, sightDist);

	for (Tile* current : tilesInSightRange)
	{
		if (current->mo_pawn)
		{
			RaycastResult ray = mo_currentMap->AmWooRaycast_Opaque(Vector2(getTileCoords()) + Vector2(.5f, .5f), Vector2(current->m_tileCoords) + Vector2(.5f, .5f));
			if(!ray.didImpact)
				m_visablePawns.push_back(&current->mo_pawn);
		}
	}
}
