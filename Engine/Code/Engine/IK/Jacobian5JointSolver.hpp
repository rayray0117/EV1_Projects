#pragma once
#include "Engine/IK/IKSolver.hpp"
#include "Engine/Math/Matrix4.hpp"
//////////////////////////////////////////////////////////////////////////
// Note: This solver is limited to 4 or 5 joints. If there are 5 joints, the end effector joint is not added into the Jacobian matrix. 
//       This limitation allows me to use my engines Matrix4 class no issues.
//////////////////////////////////////////////////////////////////////////
class Jacobian5JointSolver : public IKSolver
{
public:
	Jacobian5JointSolver() {}
	~Jacobian5JointSolver() {}
	

	virtual void Solve(IKChain* chain, const Vector3& goal) override;
	virtual void SetNumIterations(uint number) override { maxIterations = number; }
	virtual uint getNumIterations() const { return maxIterations; }
public:
	
protected:
	void ComputeJacobian(IKChain* chain, const Vector3& goal);
	void CalcDeltaThetas();
	void UpdateThetas(IKChain* chain);

protected:
	Matrix4 JacobianMatrix;
	Vector3 dT;
	Vector4 deltaThetas;
	Vector3 dS; //goal-endEffectorPos global
	Vector3 initialAxes[4];
	Vector4 thetas;

private:
	const float dampening = .01f; //Number needed to keep system from exploding.
	uint maxIterations = 1000;

	void AddVectorToJacobianMatrix(uint i, const Vector3& temp);
private:
	Vector3 GetDeltaOrientation(IKChain* chain, const Vector3& goal);
	Matrix4 GetJacobianTranspose(IKChain* chain);
};