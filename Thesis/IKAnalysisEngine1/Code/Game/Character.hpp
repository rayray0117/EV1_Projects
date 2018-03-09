#pragma once
#include "Engine/Renderer/Rig.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/XMLParseHelper.h"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class IKRig;
class IKMotion;
class Motion;
class IKSolver;
//////////////////////////////////////////////////////////////////////////
class Character
{
public:
	Character(const XMLNode& element);
	~Character();
	
	void Update(float deltaSeconds);
	void Render() const;

	void SetSolverForAll(IKSolver* solver);

	Motion* getMotion(const std::string& name) const;

public:
	std::string m_name = "";

	Rig m_rig;
	std::vector<Motion*> m_motions;

	IKRig* m_ikRig = nullptr;
	std::vector<IKMotion*> m_ikMotions;


	float m_motionTime = 0.f;
	uint m_motionIndex = 0;
	bool m_useIKMotion = false;
};