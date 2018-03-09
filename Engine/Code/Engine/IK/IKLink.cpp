#include "Engine/IK/IKLink.hpp"
#include "Engine/Core/Profiler.hpp"
#include "../Core/ParseUtils.hpp"
#include "../Core/StringUtils.hpp"
#include "../Core/ErrorWarningAssert.hpp"
//////////////////////////////////////////////////////////////////////////
void IKLink::SetRoot(IKJoint* root)
{
	m_root = root;
	// May want to use SQT? Could be easier to get position and less data needed to store transform. Might be more accurate?
	Vector3 rootPos = root->getPosition_Global();
	Vector3 endPos = m_joint.getPosition_Global();

	m_length = calcDistance(rootPos, endPos);
}

SQT IKLink::getLocalTransformForCCD() const
{
	//PROFILE_SCOPE_FUNCTION();
	//Matrix4 localTransform = m_joint.local_transform.getAsMatrix();
	/*/
	Matrix4 localTransform = m_joint.global_transform; //Possible Dirty fix?
	if (m_root != nullptr)
	{
		//Matrix4 inverseParentTransform = m_root->global_transform.getInverse();
		//localTransform = m_joint.global_transform.getTransformed(inverseParentTransform);
		localTransform = m_joint.local_transform.getAsMatrix();
	}
	//*/
	return m_joint.local_transform;
}

void IKLink::SetFromLocalTransformForCCD(const SQT& /*myLocalTransform*/)
{
	//PROFILE_SCOPE_FUNCTION();
	//m_joint.local_transform.setFromMatrix(myLocalTransform);
	//*/
	if (m_root != nullptr)
	{
		m_joint.parents_global_transform = m_root->getGlobalTransform();
		return;
	}
	else
	{
		//m_joint.global_transform = myLocalTransform; //Possible Dirty fix?
	}
}

void IKLink::UpdateJointsParentTransForm()
{
	//Roots get updated manually so skip this if you are a root.
	if (m_root != nullptr)
		m_joint.parents_global_transform = m_root->getGlobalTransform();
}

void IKLink::SetJointsPosition_Global(const Vector3& global_position)
{
	if (m_root == nullptr) return;

	//m_joint.SetPosition_Global(global_position);
	
	//*/
	if (m_root == nullptr)
	{
		m_joint.SetPosition_Global(global_position);
		return;
	}

	SQT parentGlobalTrans = m_root->getGlobalTransform();
	m_joint.parents_global_transform = parentGlobalTrans;

	//*
	Vector3 currentGlobalPos = m_joint.getGlobalTransform().position;
	Vector3 parentGlobalPos = parentGlobalTrans.position;

	Vector3 dirParentToCurrent = getDirection(parentGlobalPos, currentGlobalPos);
	Vector3 dirParentToDesired = getDirection(parentGlobalPos, global_position);

	Matrix4 rotationMat = parentGlobalTrans.getInverse().rotation.getAsMatrix();
	dirParentToCurrent = rotationMat.TransformDirection(dirParentToCurrent);
	dirParentToDesired = rotationMat.TransformDirection(dirParentToDesired);

	float cosAngle = dotProduct(dirParentToCurrent, dirParentToDesired);
	if (cosAngle < 0.9999f)
	{
		Vector3 rotationalAxis = crossProduct(dirParentToCurrent, dirParentToDesired);
		rotationalAxis.normalize();

		float turnAngle =  EV1::RAD2DEG * acosf(cosAngle);

		Quaternion rotateBy(rotationalAxis, turnAngle);
		rotateBy.Normalize();
		parentGlobalTrans.rotation = parentGlobalTrans.rotation * rotateBy;
		m_root->SetRotation_Global(parentGlobalTrans.rotation);


		m_joint.parents_global_transform = m_root->getGlobalTransform();
		
// 		ASSERT_RECOVERABLE(global_position.isMostlyEqualTo(m_joint.getPosition_Global()),
// 			C_STRF("global transform check failed result:  joint = %s, desired = %s.", 
// 				vectorToString(m_joint.getPosition_Global()).c_str(),
// 				vectorToString(global_position).c_str()));
	}
	//*/
}

//////////////////////////////////////////////////////////////////////////