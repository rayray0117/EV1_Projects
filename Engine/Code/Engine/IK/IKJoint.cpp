#include "Engine/IK/IKJoint.hpp"
//////////////////////////////////////////////////////////////////////////
IKJoint::IKJoint(const Joint& copy, Skeleton* o_skeleton)
	: IKJoint(copy)
{
	parents_global_transform.setFromMatrix(o_skeleton->getJointTransform(parentName.c_str()));

	SQT inverseParentTransform = parents_global_transform.getInverse();
	SQT globalTrans;
	globalTrans.setFromMatrix(copy.global_transform);
	local_transform = globalTrans.getCombined(inverseParentTransform);
}

void IKJoint::SetGlobalTransform(const SQT& global, const SQT& parentGlobal)
{
	parents_global_transform = parentGlobal;

	SQT inverseParentTransform = parents_global_transform.getInverse();
	local_transform.position = global.getCombined(inverseParentTransform).position;
}

//////////////////////////////////////////////////////////////////////////
/*/
void IKJoint::SetLocalFromParent(const Matrix4& parent_global)
{
	Matrix4 inverseParentTransform = parent_global.getInverse();
	Matrix4 local = global_transform.getTransformed(inverseParentTransform);
	local_transform.setFromMatrix(local);
}
void IKJoint::SetGlobalFromParent(const Matrix4& parent_global)
{
	Matrix4 local = local_transform.getAsMatrix();
	global_transform = local.getTransformed(parent_global);
}
//*/
//////////////////////////////////////////////////////////////////////////
void IKJoint::SetPosition_Global(const Vector3& position)
{
	SQT globalTrans = getGlobalTransform();
	globalTrans.position = position;

	SQT inverseParentTransform = parents_global_transform.getInverse();
	local_transform.position = globalTrans.getCombined(inverseParentTransform).position;
}

void IKJoint::SetRotation(const Quaternion& rotation)
{
	local_transform.rotation = rotation;
}

void IKJoint::SetRotation_Global(const Quaternion& rotation)
{
	SQT globalTrans = getGlobalTransform();
	globalTrans.rotation = rotation;

	SQT inverseParentTransform = parents_global_transform.getInverse();
	local_transform.rotation = globalTrans.getCombined(inverseParentTransform).rotation;
}

const SQT IKJoint::getGlobalTransform() const
{
	return local_transform.getCombined(parents_global_transform);
}

const Vector3 IKJoint::getPosition_Global() const
{
	SQT globalTrans = getGlobalTransform();
	return globalTrans.position;
}

Quaternion IKJoint::getRotation_Global() const
{
	SQT globalTrans = getGlobalTransform();
	return globalTrans.rotation;
}
