#pragma once
#include "Game/Behavior.hpp"
#include "Game/Map.hpp"
//////////////////////////////////////////////////////////////////////////
class WanderBehavior : public Behavior
{
public:
	WanderBehavior(const XMLNode& element);
	WanderBehavior(const WanderBehavior& copy);
	~WanderBehavior() {};
	
	virtual void Act(Pawn& pawn) override;
	virtual Behavior* Clone() const override;
	void DebugRender() const;

	float CalcUtility(Pawn& pawn) const;
public:
	Path m_currentPath;
	uint m_maxNumberOfWastedTurns; //How many extra turns can I use trying to reach my goal before giving up and going somewhere else
private:
	void Wander_RandomDirection(Pawn& pawn);

	void Wander_RandomMapPosPath(Pawn& pawn);
	void PickNewRandomMapPosPath(Pawn& pawn);

private:
	uint m_estTurnsNeededToReachGoal = 0;
};
