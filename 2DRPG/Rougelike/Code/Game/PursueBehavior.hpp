#pragma once
#include "Game/Behavior.hpp"
#include "Game/Map.hpp"
//////////////////////////////////////////////////////////////////////////
class PursueBehavior : public Behavior
{
public:
	PursueBehavior(const XMLNode& element);
	PursueBehavior(const PursueBehavior& copy);
	~PursueBehavior() {}
	
	void DebugRender() const;
	virtual void Act(Pawn& pawn) override;
	virtual Behavior* Clone() const override;
	float CalcUtility(Pawn& pawn) const override;
public:
	
private:
	void Pursue_RandomDirectionTowardFirstPlayer(Pawn& pawn);
	void Pursue_AStarToTarget(Pawn& pawn);
private:
	Path m_currentPath;
};
