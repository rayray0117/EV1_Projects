#include "Engine/IK/IKSolver.hpp"
#include "Engine/IK/IKChain.hpp"
//////////////////////////////////////////////////////////////////////////
bool IKSolver::isCloseEnough(IKChain* chain, const Vector3& goal, float epsilon /*= 0.001f*/) const
{
	float sqDist = calcDistanceSquared(goal, chain->getEndEffectorPosition_Global());
	//goal.isMostlyEqualTo(chain->getEndEffectorPosition_Global(), epsilon);
	return sqDist <= (epsilon*epsilon);
}
