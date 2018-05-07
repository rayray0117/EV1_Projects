#include "Engine/IK/TwoBoneSolver.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/IK/IKChain.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "../Core/ErrorWarningAssert.hpp"
#include "../Core/StringUtils.hpp"
#include "../Core/ParseUtils.hpp"
#include "IKLink.hpp"
//////////////////////////////////////////////////////////////////////////
void TwoBoneSolver::Solve(IKChain* chain, const Vector3& goal)
{
	Vector3 RootPos = chain->getRootPosition_Global();
	Vector3 JointPos = chain->getLinksPosition_Global(1);
	// This is our reach goal.
	Vector3 DesiredPos = goal;
	Vector3 DesiredDelta = DesiredPos - RootPos;
	float DesiredLength = DesiredDelta.calcLength();
	float LowerLimbLength = chain->getLinksLength(0);
	float UpperLimbLength = chain->getLinksLength(1);
	// Find lengths of upper and lower limb in the ref skeleton.
	// Use actual sizes instead of ref skeleton, so we take into account translation and scaling from other bone controllers.
	float MaxLimbLength = LowerLimbLength + UpperLimbLength;

	// Check to handle case where DesiredPos is the same as RootPos.
	Vector3	DesiredDir;
	if (DesiredLength < (float)(1.e-4f))
	{
		DesiredLength = (float)(1.e-4f);
		DesiredDir = Vector3(1, 0, 0);
	}
	else
	{
		DesiredDir = DesiredDelta.getNormalizedVector();
	}


	Vector3 JointBendDir = chain->getMidJointDirection();


	//UE_LOG(LogAnimationCore, Log, TEXT("UpperLimb : %0.2f, LowerLimb : %0.2f, MaxLimb : %0.2f"), UpperLimbLength, LowerLimbLength, MaxLimbLength);
	Vector3 OutEndPos = DesiredPos;
	Vector3 OutJointPos = JointPos;

	// If we are trying to reach a goal beyond the length of the limb, clamp it to something solvable and extend limb fully.
	if (DesiredLength >= MaxLimbLength)
	{
		OutEndPos = RootPos + (MaxLimbLength * DesiredDir);
		OutJointPos = RootPos + (UpperLimbLength * DesiredDir);
	}
	else
	{
		// So we have a triangle we know the side lengths of. We can work out the angle between DesiredDir and the direction of the upper limb
		// using the sin rule:
		const float TwoAB = 2.f * UpperLimbLength * DesiredLength;

		const float CosAngle = (TwoAB != 0.f) ? ((UpperLimbLength*UpperLimbLength) + (DesiredLength*DesiredLength) - (LowerLimbLength*LowerLimbLength)) / TwoAB : 0.f;

		// If CosAngle is less than 0, the upper arm actually points the opposite way to DesiredDir, so we handle that.
		const bool bReverseUpperBone = (CosAngle < 0.f);

		// Angle between upper limb and DesiredDir
		// ACos clamps internally so we dont need to worry about out-of-range values here.
		const float Angle = acos(CosAngle);

		// Now we calculate the distance of the joint from the root -> effector line.
		// This forms a right-angle triangle, with the upper limb as the hypotenuse.
		const float JointLineDist = UpperLimbLength * sin(Angle);

		// And the final side of that triangle - distance along DesiredDir of perpendicular.
		// ProjJointDistSqr can't be neg, because JointLineDist must be <= UpperLimbLength because appSin(Angle) is <= 1.
		const float ProjJointDistSqr = (UpperLimbLength*UpperLimbLength) - (JointLineDist*JointLineDist);
		// although this shouldn't be ever negative, sometimes Xbox release produces -0.f, causing ProjJointDist to be NaN
		// so now I branch it. 						
		float ProjJointDist = (ProjJointDistSqr > 0.f) ? sqrt(ProjJointDistSqr) : 0.f;
		if (bReverseUpperBone)
		{
			ProjJointDist *= -1.f;
		}

		// So now we can work out where to put the joint!
		OutJointPos = RootPos + (ProjJointDist * DesiredDir) + (JointLineDist * JointBendDir);
	}

	chain->m_links[1]->SetJointsPosition_Global(OutJointPos);
	chain->m_links[0]->SetJointsPosition_Global(OutEndPos);

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
	

	
	/*/
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
	
	/*
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
	//*/
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
