//This is what Squirel would call an actor. But I use UE4 too much to want to change naming conventions.
#pragma once
#include "Game/Actor.hpp"

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/IntVector2.hpp"

#include "Game/PawnDefinition.hpp"
//////////////////////////////////////////////////////////////////////////
class Controller;
class Tile;
class Item;
class Behavior;
//////////////////////////////////////////////////////////////////////////
class Pawn : public Actor
{
public:
	Pawn();
	Pawn(const std::string pawnDefName);
	Pawn(const std::string pawnDefName, uint controllerId);
	~Pawn();

	void DebugRender() const;
	virtual void Act();

    bool MoveToTile(Tile* tile);
    bool MoveToTile(const IntVector2& tileCoords);
	bool AttackTile(Tile* tile);
	bool AttackTile(const IntVector2& tileCoords);

    void AssignController(uint controllerId);

	const Stats getCombinedStats() const;
	float getCurrentHealthPercent() const;

	void FindTarget();

	//Battle functions
	void takeDirectDamage(int damage); //Just take the damage you get and don't have a fit.

	void setVisibleTiles(int mapSightBonus);
	void setVisibleActors();
public:
	Controller* mo_controller = nullptr;
	Item* mo_equipment[NUM_EQSLOTS];
	std::string m_faction;
	std::vector<Behavior*> m_behaviors;
	PawnDefinition* mo_def = nullptr;
	Pawn* mo_target = nullptr;

private:
	std::string m_tookDamageSoundEffect;
	std::vector<Pawn**> m_visablePawns;
};