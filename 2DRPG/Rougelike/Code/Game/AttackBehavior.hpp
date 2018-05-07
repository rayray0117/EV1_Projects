#pragma once
#include "Game/Behavior.hpp"
//////////////////////////////////////////////////////////////////////////
class AttackBehavior : public Behavior
{
public:
	AttackBehavior(const XMLNode& element);
	AttackBehavior(const AttackBehavior& copy);
	~AttackBehavior() {}

	void Act(Pawn& pawn) override;
	Behavior* Clone() const override;
	float CalcUtility(Pawn& pawn) const override;
public:
	
};