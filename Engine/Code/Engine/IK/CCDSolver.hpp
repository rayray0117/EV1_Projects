#pragma once
#include "Engine/IK/IKSolver.hpp"
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
private:
	uint maxIterations = 500;
private:
	//virtual bool isCloseEnough(IKChain* chain, const Vector3& goal, float epsilon = 0.001f) const override;
};