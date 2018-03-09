#include "Engine/IK/TwoBoneSolver.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/IK/IKChain.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "../Core/ErrorWarningAssert.hpp"
#include "../Core/StringUtils.hpp"
#include "../Core/ParseUtils.hpp"
//////////////////////////////////////////////////////////////////////////
void TwoBoneSolver::Solve(IKChain* chain, const Vector3& goal)
{
	/*/
	float length, lengthSqr, lengthInv, x, y;
	Vector3 vec0, vec1;

	vec0 = goal - chain->getRootPosition_Global();
	lengthSqr = vec0.calcLengthSquared();
	lengthInv = inverseSqrt_f(lengthSqr);
	length = lengthInv * lengthSqr;

	float len0 = chain->getLinksLength(1);
	float len1 = chain->getLinksLength(0);

	Vector3 midJointPos;

	// if the start and end position are too far out or too close to each other
	if (length > len0 + len1 || length < fabsf(len0 - len1)) 
	{
		midJointPos = chain->getRootPosition_Global() + 0.5f * vec0;
		return;
	}

	vec0 *= lengthInv;
	vec1 = chain->getMidJointDirection() - vec0 * chain->getMidJointDirection() * vec0;
	vec1.normalize();

	x = (length * length + len0 * len0 - len1 * len1) * (0.5f * lengthInv);
	y = sqrtf(len0 * len0 - x * x);

	midJointPos = chain->getRootPosition_Global() + x * vec0 + y * vec1;

	return;
	//*/

	//My navie attempt at two bone analytics
	//*/
	

	

	float rootToMidLength = chain->getLinksLength(1);
	float midToEndLength = chain->getLinksLength(0);
	float rootToEndLength = calcDistance(chain->getRootPosition_Global(), goal);


	float rootOrientation;
	float midOrientation;

	float rootCosAngle = ((rootToMidLength * rootToMidLength) + (rootToEndLength * rootToEndLength) - (midToEndLength*midToEndLength)) / (2 * rootToMidLength*rootToEndLength);
	float midCosAngle = ((midToEndLength * midToEndLength) + (rootToMidLength * rootToMidLength) - (rootToEndLength*rootToEndLength)) / (2 * midToEndLength*rootToMidLength);

	rootOrientation = EV1::RAD2DEG * acosf(rootCosAngle);
	midOrientation = EV1::RAD2DEG * acosf(midCosAngle);


	

	Vector3 dirFromRootToGoal = getDirection(chain->getRootPosition_Global(), goal); //Get direction in local to root space?
	Vector3 midDirection = chain->getMidJointDirection();
	Matrix4 rotationMatMid = chain->getLinksJoint(1)->getGlobalTransform().getInverse().rotation.getAsMatrix();
	
	//*/
	{
		Matrix4 rotationMatRoot = chain->getLinksJoint(2)->getGlobalTransform().getInverse().rotation.getAsMatrix();
		Vector3 dirFromRootToGoalRoot = rotationMatRoot.TransformDirection(dirFromRootToGoal);
		Vector3 midDirectionRoot = rotationMatRoot.TransformDirection(midDirection);

		Vector3 rotationalAxis = crossProduct(midDirectionRoot, dirFromRootToGoalRoot);
		rotationalAxis.normalize();
		
		Vector3 euler =Rotator::AxisAngleToEuler(rotationalAxis, EV1::DEG2RAD * rootOrientation);
		Quaternion rootRot;
		rootRot.SetFromEuler(euler);
		Quaternion worldRot = chain->getRootJoint()->getGlobalTransform().rotation;
		worldRot *= rootRot;
		chain->getRootJoint()->SetRotation(rootRot);
		chain->UpdateAllParentTransforms();
	}
	/*/
	{
		
		Vector3 dirFromRootToGoalRoot = rotationMatMid.TransformDirection(dirFromRootToGoal);
		Vector3 midDirectionRoot = rotationMatMid.TransformDirection(midDirection);

		Vector3 rotationalAxis = crossProduct(dirFromRootToGoalRoot, midDirectionRoot);
		rotationalAxis.normalize();

		Quaternion midRot(rotationalAxis, midOrientation);
		Quaternion worldRot = chain->getLinksJoint(1)->getGlobalTransform().rotation;
		worldRot *= midRot;
		chain->getLinksJoint(1)->SetRotation(midRot);
		chain->UpdateAllParentTransforms();
	}
	return;
	//*/
}
