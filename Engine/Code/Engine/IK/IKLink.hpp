#pragma once
#include "Engine/IK/IKJoint.hpp"
//////////////////////////////////////////////////////////////////////////
class IKLink
{
public:
	IKLink() {}
	//IKLink(const Joint& copy);
	~IKLink() {}

	void SetRoot(IKJoint* root);

	//Currently just to help with CCD rotations.
	SQT getLocalTransformForCCD() const;
	void SetFromLocalTransformForCCD(const SQT& myLocalTransform);
	void UpdateJointsParentTransForm();
	void SetJointsPosition_Global(const Vector3& global_position);
public:
	IKJoint* m_root = nullptr; //Have this, not sure if I need it.
	IKJoint m_joint; //End effector
	//Vector3 bone; // The direction and distance from the root to the end
	float m_length;
};