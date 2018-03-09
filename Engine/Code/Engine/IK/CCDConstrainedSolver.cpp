#include "Engine/IK/CCDConstrainedSolver.hpp"
#include "Engine/IK/IKChain.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Transform.hpp"
#include "IKConstraint.hpp"
//////////////////////////////////////////////////////////////////////////
void CCDConstrainedSolver::Iterate(IKChain* chain, uint &currentEffectorIndex, const Vector3& endPos, uint &currentIterations)
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
		CheckConstraint(chain->getLinksJoint(currentEffectorIndex));
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

void CCDConstrainedSolver::CheckConstraint(IKJoint* jointToCheck)
{
	if (jointToCheck->m_constraint == nullptr) return;

	IKConstraint& constraint = *jointToCheck->m_constraint;
	Quaternion& qrotation = jointToCheck->local_transform.rotation;
	Vector3 euler = qrotation.getEulerAngles();

	Vector3 clampedEuler = constraint.AngleClamp(euler);	
	qrotation.SetFromEuler(clampedEuler);
}