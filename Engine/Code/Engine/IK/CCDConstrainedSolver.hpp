#pragma once
#include "Engine/IK/CCDSolver.hpp"
//////////////////////////////////////////////////////////////////////////
struct IKJoint;
class IKLink;
//////////////////////////////////////////////////////////////////////////
class CCDConstrainedSolver : public CCDSolver
{
public:
	CCDConstrainedSolver() {}
	~CCDConstrainedSolver() {}
	
protected:
	virtual void Iterate(IKChain* chain, uint &currentEffectorIndex, const Vector3& endPos, uint &currentIterations) override;
	virtual void CheckConstraint(IKJoint* jointToCheck); //Will check if there are constraints and fixed them if so.
};