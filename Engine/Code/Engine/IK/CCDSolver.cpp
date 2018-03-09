#include "Engine/IK/CCDSolver.hpp"
#include "Engine/IK/IKChain.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/General/UniversalFunction.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/Time.hpp"
//////////////////////////////////////////////////////////////////////////
void CCDSolver::Solve(IKChain* chain, const Vector3& endPos)
{
	PROFILE_SCOPE_FUNCTION();
	float beginTime = (float)GetCurrentTimeSeconds();
	//Using darwin3d's CCD3D code implementation as reference, because it is one of the more straight forward implementations I was able to find.
	uint currentIterations = 0;
	uint currentEffectorIndex = 1; //Start at the end effector

	chain->UpdateAllParentTransforms();


	while (!isCloseEnough(chain, endPos) && currentIterations < maxIterations)
	{
		Iterate(chain, currentEffectorIndex, endPos, currentIterations);
	}


	float endTime = (float)GetCurrentTimeSeconds();
	std::string str = Stringf("CCD, %s, %i, %i, %f, %f, %s, %f \n",
		chain->m_name.c_str(), currentIterations, maxIterations, (endTime - beginTime) * 1000 * 1000, IK_TOLERANCE,
		vectorToString(endPos).c_str(), calcDistance(endPos, chain->getEndEffectorPosition_Global()));
	UF->CallF("IKData", (void*)str.c_str());

	DebuggerPrintf("Chain: %s - CCD: Num iterations = %i, Max Iterations = %i \n", chain->m_name.c_str(),currentIterations, maxIterations);
}

void CCDSolver::Iterate(IKChain* chain, uint &currentEffectorIndex, const Vector3& endPos, uint &currentIterations)
{
	//PROFILE_SCOPE("Inner CCD Solve Loop");
	Vector3 rootPos = chain->getLinksPosition_Global(currentEffectorIndex);
	Vector3 curEnd = chain->getEndEffectorPosition_Global();


	Vector3 curVector = getDirection(rootPos, curEnd);
	Vector3 targetVector = getDirection(rootPos, endPos);

	Matrix4 rotationMat = chain->getLinksJoint(currentEffectorIndex)->getGlobalTransform().getInverse().rotation.getAsMatrix();
	curVector = rotationMat.TransformDirection(curVector);
	targetVector = rotationMat.TransformDirection(targetVector);

	
	//*/
	float cosAngle = dotProduct(curVector, targetVector);
	if (cosAngle < /*1.f*/ 0.9999f) //Jan 5 2017: Found that if the angle is almost equal to one, then the cross product will return a ZERO vector. In my references they used .9999 instead, presumably to avoid this issue. Will copy this method and debate later about whether or not to do more correct checks, such as cosAngle isMostlyEqualTo 1.f or is the crossproduct == to ZERO.
	{
		Vector3 cross = crossProduct(curVector, targetVector);
		cross.normalize();
		float turnAngle = convertRadiansToDegrees(acos(cosAngle));

		Quaternion q(cross, turnAngle);
		q.Normalize();
		q = chain->getLinksJoint(currentEffectorIndex)->getRotation_Global() * q;
		//localTrans[currentEffectorIndex].rotation *= q;
		chain->getLinksJoint(currentEffectorIndex)->SetRotation_Global(q);
	}
	//*/
	//*/
	//Start from the current effector and work down to the end effector
	//for (int i = currentEffectorIndex; i >= 0; --i)
	chain->UpdateAllParentTransforms();
	//*/
	++currentEffectorIndex;
	if (currentEffectorIndex > chain->getNumberOfLinks() - 1) //Greater than or equal to because if it actually is equal to the size it'll be out of bounds in the vector.
	{
		currentEffectorIndex = 1;
		++currentIterations;
	}
}

//bool CCDSolver::isCloseEnough(IKChain* chain, const Vector3& goal, float epsilon /*= 0.001f*/) const
//{
//	return goal.isMostlyEqualTo(chain->getEndEffectorPosition(), epsilon);
//}
