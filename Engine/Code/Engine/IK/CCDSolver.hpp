#pragma once
#include "Engine/IK/IKSolver.hpp"
//////////////////////////////////////////////////////////////////////////
struct IKJoint;
//////////////////////////////////////////////////////////////////////////
class CCDSolver : public IKSolver
{
public:
	CCDSolver(){}
	~CCDSolver(){}
	

	virtual void Solve(IKChain* chain, const Vector3& goal) override;

	virtual void SetNumIterations(uint number) override { maxIterations = number; }
	virtual uint getNumIterations() const { return maxIterations; }
protected:
	virtual void Iterate(IKChain* chain, uint &currentEffectorIndex, const Vector3& endPos, uint &currentIterations);

	void ConventionalStep(IKChain* chain, uint &currentEffectorIndex, const Vector3& endPos, uint &currentIterations); //Start from end-1 to root
	void AlternativeStep(IKChain* chain, uint &currentEffectorIndex, const Vector3& endPos, uint &currentIterations); //Start from root to end-1

	virtual void CheckConstraint(IKJoint* jointToCheck); //Will check if there are constraints and fixed them if so.
protected:
	bool m_constrained = false;
	byte_t m_typeFlag = 1; //1 = Conventional, 2 = Alternate, 3 = Conventional Then Alternate
private:
	//uint maxIterations = 50;
private:
	//virtual bool isCloseEnough(IKChain* chain, const Vector3& goal, float epsilon = 0.001f) const override;
};

class AltCCDSolver : public CCDSolver
{
public:
	AltCCDSolver() { m_typeFlag = 2; }
	~AltCCDSolver() {}
};

class CACCDSolver : public CCDSolver
{
public:
	CACCDSolver() { m_typeFlag = 3; }
	~CACCDSolver() {}
};