#include "Engine/IK/IKLink.hpp"
#include "Engine/Core/Profiler.hpp"
#include "../Core/ParseUtils.hpp"
#include "../Core/StringUtils.hpp"
#include "../Core/ErrorWarningAssert.hpp"
#include "../Math/MathUtils.hpp"
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
	return m_joint.local_transform;
}

void IKLink::SetFromLocalTransformForCCD(const SQT& /*myLocalTransform*/)
{
	if (m_root != nullptr)
	{
		m_joint.parents_global_transform = m_root->getGlobalTransform();
		return;
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
	/*/
	SQT parentGlobalTrans = m_root->getGlobalTransform();
	Vector3 parentGlobalPos = parentGlobalTrans.position;
	//Vector3 dirParentToCurrent = getDirection(parentGlobalPos, currentGlobalPos);
	Vector3 dirParentToDesired = getDirection(parentGlobalPos, global_position);
	Matrix4 rotationMat = parentGlobalTrans.getInverse().rotation.getAsMatrix();
	//dirParentToCurrent = rotationMat.TransformDirection(dirParentToCurrent);
	dirParentToDesired = rotationMat.TransformDirection(dirParentToDesired);
	Matrix4 rotMatirx = Matrix4::CreateLookAtMatrix(parentGlobalTrans.position, (parentGlobalTrans.position + dirParentToDesired), rotationMat.TransformDirection(Vector3::YAXIS));
	//Matrix4 rotMatirx = Matrix4::CreateRotationFromDirection(dirParentToDesired, rotationMat.TransformDirection(Vector3::ZAXIS));
	parentGlobalTrans.rotation = Quaternion(rotMatirx);
	m_root->SetRotation_Global(parentGlobalTrans.rotation);
	m_joint.parents_global_transform = m_root->getGlobalTransform();
	/*/

	//*/
	//{//SolveAim Attempt
	//	SQT parentGlobalTrans = m_root->getGlobalTransform();
	//	m_joint.parents_global_transform = parentGlobalTrans;
	//
	//	Vector3 currentGlobalPos = m_joint.getGlobalTransform().position;
	//	Vector3 parentGlobalPos = parentGlobalTrans.position;
	//
	//	Vector3 dirParentToCurrent = getDirection(parentGlobalPos, currentGlobalPos);
	//	Matrix4 toRootSpaceTransform = parentGlobalTrans.getAsMatrix().getInverse(); //Matrix for Root's axis
	//	dirParentToCurrent = toRootSpaceTransform.TransformDirection(dirParentToCurrent);
	//	Vector3 currentLocalPos = toRootSpaceTransform.TransformPosition(currentGlobalPos);
	//	Vector3 parentLocalPos = toRootSpaceTransform.TransformPosition(parentGlobalPos);
	//	Vector3 localTargetPos = toRootSpaceTransform.TransformPosition(global_position);
	//	Quaternion detlaRotate = Quaternion::SolveAim(currentLocalPos, localTargetPos, dirParentToCurrent);
	//	m_root->SetRotation_Global(parentGlobalTrans.rotation * detlaRotate);
	//	m_joint.parents_global_transform = m_root->getGlobalTransform();
	//}
	//*/

	//*/
	{//Default Working solution
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

			float turnAngle = EV1::RAD2DEG * acosf(cosAngle);

			//Perform Rotation
			Quaternion rotateBy(rotationalAxis, turnAngle);
			rotateBy.Normalize();
			parentGlobalTrans.rotation = m_root->local_transform.rotation * rotateBy;
			//parentGlobalTrans.rotation.Normalize();
			m_root->SetRotation(parentGlobalTrans.rotation);

			/*/
			{//Clamp the X Axis rotation for FABRIK since it likes to spin for some reason when you do this stuff.
				Quaternion& qrotation = m_root->local_transform.rotation;
				Vector3 euler = qrotation.getEulerAngles();
			
				Vector3 clampedEuler = euler;
				if(m_root->parentName == "root") //HACK fix for UE4 guys pelvis
					clampedEuler.z = 0.f;
				else
					clampedEuler.x = 0.f;
				m_root->local_transform.rotation.SetFromEuler(clampedEuler);
			}
			//*/

			m_joint.parents_global_transform = m_root->getGlobalTransform();
		}
	}
	//*/
}

//////////////////////////////////////////////////////////////////////////