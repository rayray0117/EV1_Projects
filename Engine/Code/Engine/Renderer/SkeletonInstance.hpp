#pragma once
#include "Engine/Renderer/Pose.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Motion.hpp"
//////////////////////////////////////////////////////////////////////////
class Skeleton;
class StructuredBuffer;
//////////////////////////////////////////////////////////////////////////
class SkeletonInstance
{
public:
	SkeletonInstance();
	~SkeletonInstance();

	// Excercise:  How do we evaluate a global transform for this joint if the pose only contains 
	// local transforms?
	// Hint:  You will need "get_joint_parent" below.
	SQT get_joint_global_transform(uint joint_idx) const;
	SQT get_joint_global_transform(const std::string& jointName) const;

	SQT get_joint_local_transform(uint joint_idx) const;
	SQT get_joint_local_transform(const std::string& jointName) const;

	// returns INVALID_INDEX (-1) if no parent exists.
	// Use the skeleton to get this information.
	int get_joint_parent(uint joint_idx) const;
	int get_joint_parent(const std::string& joint_idx) const;

	void apply_motion(const Motion* motion, float const time)
	{
		if (motion == nullptr)
			return;
		motion->evaluate(&m_pose, time);
	}

	void apply_motion(const Motion* motion, uint const frame)
	{
		if (motion == nullptr)
			return;
		motion->evaluate(&m_pose, frame);
	}

	void SetStructedBuffer(StructuredBuffer* buffer);
	void DeleteStructuredBuffer();
public:
	Skeleton* mo_skeleton; // skeleton we're applying poses to.  Used for hierarchy information.
	Pose m_pose;  // my current skeletons pose.
	StructuredBuffer* m_skinTransfroms;

public: //Debugging and More Control Features. Use with caution.
	void SetJointsTransform(const std::string& jointName, const SQT& local_transform);
	void SetJointsTransform(uint jointIndex, const SQT& local_transform);
};