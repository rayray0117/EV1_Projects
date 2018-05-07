#pragma once
#include "Game/Behavior.hpp"
#include "Game/Map.hpp"
//////////////////////////////////////////////////////////////////////////
class FleeBehavior : public Behavior
{
public:
	FleeBehavior(const XMLNode& element);
	FleeBehavior(const FleeBehavior& copy);
	~FleeBehavior() {}
	
	void DebugRender() const;
	virtual void Act(Pawn& pawn) override;
	virtual Behavior* Clone() const override;
	float CalcUtility(Pawn& pawn) const override;
public:
	void Flee_RandomDirectionAway(Pawn& pawn);
	void Flee_RandomPathAway(Pawn& pawn);

private:
	Path m_currentPath;
};