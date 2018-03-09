#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class IKChain;
class Vector3;
//////////////////////////////////////////////////////////////////////////
const float IK_TOLERANCE = 1.5f;
//////////////////////////////////////////////////////////////////////////
class IKSolver
{
public:
	IKSolver() {}
	~IKSolver() {}
	
	virtual void Solve(IKChain* chain, const Vector3& goal) = 0;

	//Mostly for debugging
	virtual void SetNumIterations(uint /*number*/) {} //Doing it this way allows me to debug easier without forcing every solver to have a max number of iterations if they aren't iterative. And if all of them are, then I can still use this function after making changes.
	virtual uint getNumIterations() const { return 0; }
public:
	
protected:
	virtual bool isCloseEnough(IKChain* chain, const Vector3& goal, float epsilon = IK_TOLERANCE) const;
};