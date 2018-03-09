#include "Engine/Renderer/SkeletonInstance.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/RHI/StructuredBuffer.hpp"
//////////////////////////////////////////////////////////////////////////
SkeletonInstance::SkeletonInstance()
	: mo_skeleton(nullptr)
	, m_skinTransfroms(nullptr)
{
}

SkeletonInstance::~SkeletonInstance()
{
	SAFE_DELETE(m_skinTransfroms);
}

SQT SkeletonInstance::get_joint_global_transform(uint joint_idx) const
{
	SQT mytrans = m_pose.m_transforms[joint_idx];
	int parentIndex = get_joint_parent(joint_idx);
	if (parentIndex == -1)
	{
		return mytrans;
	}
	else
	{
		SQT parentTrans = get_joint_global_transform(parentIndex);
		return mytrans.getCombined(parentTrans);
	}
}

int SkeletonInstance::get_joint_parent(uint joint_idx) const
{
	if (!mo_skeleton)
		return -1;

	Joint currentJoint = mo_skeleton->m_joints[joint_idx];

	if (currentJoint.parentName.empty())
		return -1;

	return mo_skeleton->getJointIndex(currentJoint.parentName.c_str());
}

void SkeletonInstance::SetStructedBuffer(StructuredBuffer* buffer)
{
	DeleteStructuredBuffer(); //Delete for safety. SkelInsts own and are responsible for their buffers.
	m_skinTransfroms = buffer;
}

void SkeletonInstance::DeleteStructuredBuffer()
{
	SAFE_DELETE(m_skinTransfroms);
}
