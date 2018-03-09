#pragma once
#include "Engine/IK/IKSolver.hpp"
//////////////////////////////////////////////////////////////////////////
// A Simple Two Bone Analytic Solver
// Note: DO NOT USE ON CHAINS WITH MORE THAN 3 JOINTS!
// This solver assume there are only 2 bones (3 Joints) and makes key assumptions from that!
//////////////////////////////////////////////////////////////////////////
class TwoBoneSolver : public IKSolver
{
public:
	TwoBoneSolver() {}
	~TwoBoneSolver() {}
	

	virtual void Solve(IKChain* chain, const Vector3& goal) override;

public:
	
};