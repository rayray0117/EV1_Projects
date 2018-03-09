#pragma once
#include "Engine/IK/IKSolver.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class FABRIKSolver : public IKSolver
{
public:
	FABRIKSolver() {}
	~FABRIKSolver() {}
	

	virtual void Solve(IKChain* chain, const Vector3& goal) override;

	void CollectGlobals(IKChain* chain);

	void ApplyTransforms(IKChain* chain);

	virtual void SetNumIterations(uint number) override { maxIterations = number; }
	virtual uint getNumIterations() const { return maxIterations; }
private:
	uint maxIterations = 500;
	std::vector<Vector3> m_globalPositions;
private:
	virtual bool isCloseEnough(IKChain* chain, const Vector3& goal, float epsilon = 3.0f) const override;
	virtual bool isGoalOutOfReach(IKChain* chain, const Vector3& goal) const;
	virtual void StraightenOutToGoal(IKChain* chain, const Vector3& goal);
	virtual void Backward(IKChain* chain, const Vector3& goal);
	virtual void Forward(IKChain* chain, const Vector3& goal);
};