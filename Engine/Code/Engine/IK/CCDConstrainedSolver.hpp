#pragma once
#include "Engine/IK/CCDSolver.hpp"
//////////////////////////////////////////////////////////////////////////
struct IKJoint;
class IKLink;
//////////////////////////////////////////////////////////////////////////
class CCDConstrainedSolver : public CCDSolver
{
public:
	CCDConstrainedSolver() { m_constrained = true; }
	~CCDConstrainedSolver() {}
	
protected:
	//virtual void Iterate(IKChain* chain, uint &currentEffectorIndex, const Vector3& endPos, uint &currentIterations) override;
	//virtual void CheckConstraint(IKJoint* jointToCheck); //Will check if there are constraints and fixed them if so.
};

class AltCCDConstrainedSolver : public CCDSolver
{
public:
	AltCCDConstrainedSolver() { m_constrained = true; m_typeFlag = 2; }
	~AltCCDConstrainedSolver() {}
};

class CACCDConstrainedSolver : public CCDSolver
{
public:
	CACCDConstrainedSolver() { m_constrained = true; m_typeFlag = 3; }
	~CACCDConstrainedSolver() {}
};