#include "Engine/IK/FABRIKSolver.hpp"
#include "Engine/IK/IKChain.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/General/UniversalFunction.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/Time.hpp"
//////////////////////////////////////////////////////////////////////////
void FABRIKSolver::Solve(IKChain* chain, const Vector3& goal)
{
	CollectGlobals(chain);

	PROFILE_SCOPE_FUNCTION();
	float beginTime = (float)GetCurrentTimeSeconds();
	uint currentIterations = 0;

	if (isCloseEnough(chain, goal)) //Idea from CALIKO FARBIK. Don't try to solve if already close enough to the goal. 
	{
		return;
	}	
	
	else if (isGoalOutOfReach(chain, goal))
	{
		StraightenOutToGoal(chain, goal);
		//return;
	}
	else
	{

		while ( !isCloseEnough(chain, goal) && (currentIterations < maxIterations))
		{
			Backward(chain, goal);
			Forward(chain, goal);
			++currentIterations;
		}
		//RotationFix(chain);
	}

	ApplyTransforms(chain);

	float endTime = (float)GetCurrentTimeSeconds();
	std::string str = Stringf("FABRIK, %s, %i, %i, %f, %f, %s, %f \n",
		chain->m_name.c_str(), currentIterations, maxIterations, (endTime - beginTime)*1000*1000, IK_TOLERANCE,
		vectorToString(goal).c_str(), calcDistance(goal, chain->getEndEffectorPosition_Global()));
	UF->CallF("IKData", (void*)str.c_str());


	DebuggerPrintf("Chain: %s - FABRIK: Num iterations = %i, Max Iterations = %i \n", chain->m_name.c_str(), currentIterations, maxIterations);
	return;
}

void FABRIKSolver::CollectGlobals(IKChain* chain)
{
	m_globalPositions.clear();
	m_globalPositions.reserve(chain->getNumberOfLinks());
	for (uint i = 0; i < chain->getNumberOfLinks(); ++i)
	{
		m_globalPositions.push_back(chain->getLinksPosition_Global(i));
	}
}

void FABRIKSolver::ApplyTransforms(IKChain* chain)
{
	for (uint i = chain->getNumberOfLinks() - 1; i > 0; --i)
	{
		chain->UpdateAllParentTransforms();
		chain->m_links[i]->SetJointsPosition_Global(m_globalPositions[i]);
	}
	chain->UpdateAllParentTransforms();
	chain->m_links[0]->SetJointsPosition_Global(m_globalPositions[0]);
}

////////////////////////////////////////////////////////////////////////// PRIVATE //////////////////////////////////////////////////////////////////////////
bool FABRIKSolver::isCloseEnough(IKChain* /*chain*/, const Vector3& goal, float epsilon /*= 0.001f*/) const
{
	//return goal.isMostlyEqualTo(chain->getEndEffectorPosition_Global(), epsilon);
	float sqDist = calcDistanceSquared(goal, m_globalPositions.front());
	return sqDist <= (epsilon*epsilon);
}

bool FABRIKSolver::isGoalOutOfReach(IKChain* chain, const Vector3& goal) const
{
	//PROFILE_SCOPE_FUNCTION();
	float distanceToGoal = calcDistance(m_globalPositions.back(), goal);
	//float distanceToGoal = calcDistance(chain->getRootPosition_Global(), goal);
	float chainLength = chain->getTotalLength(); //Storing length as temp variable to make easier to debug.

	return (distanceToGoal > chainLength);
}

void FABRIKSolver::StraightenOutToGoal(IKChain* chain, const Vector3& goal)
{
	//PROFILE_SCOPE_FUNCTION();
	//Following algorithm from FABRIK: A fast iterative solver for the inverse Kinematics Problem
	//float distanceToGoal = calcDistance(chain->getRootPosition(), goal);
	//float chainLength = chain->getTotalLength(); //d_i

	std::vector<IKLink*>& links = chain->m_links;
	for (uint i = links.size() - 1; i > 0; --i) // DEPRICATED COMMENT: Do size - 1 because we don't need to move the root in the basic implementation.
	{
		Vector3 position = m_globalPositions[i];
		//Vector3 position = chain->getLinksPosition_Global(i);
		float jointDistToGoal = calcDistance(position, goal); //r_i
		float ratio = chain->getLinksLength(i - 1) / jointDistToGoal; //l_i
		
		Vector3 nextJointsNewPosition = (1 - ratio)*position + ratio*goal; //p_i+1 = (1 - l_i)p_i + l_i * t
		m_globalPositions[i-1] = (nextJointsNewPosition);
		//chain->getLinksJoint(i - 1)->SetPosition_Global(nextJointsNewPosition);
	}
}

void FABRIKSolver::Backward(IKChain* chain, const Vector3& goal)
{
	PROFILE_SCOPE_FUNCTION();
	m_globalPositions.front() = goal;
	//IKLink* end = chain->getEndEffectorLink();
	//end->m_joint.SetPosition_Global(goal);
	for (uint i = 1; i < chain->getNumberOfLinks() - 1; ++i) // Do size - 1 because we don't need to move the root in the basic implementation.
	{
		Vector3 position = m_globalPositions[i];
		float dist = calcDistance(position, m_globalPositions[i-1]);
		//Vector3 position = chain->getLinksPosition_Global(i);
		//float dist = calcDistance(position, chain->getLinksPosition_Global(i - 1));
		float ratio = chain->getLinksLength(i) / dist;

		Vector3 nextJointsNewPosition = (1 - ratio) * m_globalPositions[i - 1] + ratio * position; //p_i+1 = (1 - l_i)p_i + l_i * t
		m_globalPositions[i]=(nextJointsNewPosition);
		//Vector3 nextJointsNewPosition = (1 - ratio) * chain->getLinksPosition_Global(i - 1) + ratio * position; //p_i+1 = (1 - l_i)p_i + l_i * t
		//chain->getLinksJoint(i)->SetPosition_Global(nextJointsNewPosition);
		//chain->UpdateAllParentTransforms();
	}
}

void FABRIKSolver::Forward(IKChain* chain, const Vector3& /*goal*/)
{
	PROFILE_SCOPE_FUNCTION();
	//Because we are never actually moving the root, we don't need to reset it to the origin position.
	//IKLink* root = chain->getRootLink();
	//root->m_joint.SetPosition(goal);
	for (uint i = chain->getNumberOfLinks() - 1; i > 0; --i) // DEPRICATED COMMENT: Do size - 1 because we don't need to move the root in the basic implementation.
	{
		Vector3 position = m_globalPositions[i];
		float dist = calcDistance(position, m_globalPositions[i - 1]);
		//Vector3 position = chain->getLinksPosition_Global(i);
		//float dist = calcDistance(position, chain->getLinksPosition_Global(i - 1));
		float ratio = chain->getLinksLength(i-1) / dist;

		Vector3 nextJointsNewPosition = (1 - ratio)*position + ratio*m_globalPositions[i - 1]; //p_i+1 = (1 - l_i)p_i + l_i * t
		m_globalPositions[i-1]=(nextJointsNewPosition);
		//Vector3 nextJointsNewPosition = (1 - ratio)*position + ratio*chain->getLinksPosition_Global(i - 1); //p_i+1 = (1 - l_i)p_i + l_i * t
		//chain->getLinksJoint(i - 1)->SetPosition_Global(nextJointsNewPosition);
		//chain->UpdateAllParentTransforms();
	}
}