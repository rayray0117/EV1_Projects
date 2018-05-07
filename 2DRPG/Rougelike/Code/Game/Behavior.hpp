#pragma once
#include "Engine\Core\EngineBase.hpp"
#include <string>
//////////////////////////////////////////////////////////////////////////
struct XMLNode;
class Pawn;
//////////////////////////////////////////////////////////////////////////
class Behavior
{
public:
	Behavior() {}
	virtual ~Behavior() {}
	
	virtual void Update(float deltaSeconds) { m_timeInBehavior += deltaSeconds; }
	virtual void Act(Pawn& pawn) = 0;
	virtual void DebugRender() const {};

	virtual float CalcUtility(Pawn& pawn) const { UNUSED(pawn); return 0.f; }

	virtual Behavior* Clone() const = 0;
	static Behavior* CreateBehavior(const XMLNode& element);
public:
	float m_timeInBehavior = 0.f;
	uint m_turnsInBehavior = 0;
	std::string m_name;
};
