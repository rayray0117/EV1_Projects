#pragma once
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Core/EngineBase.hpp"
#include <vector>
#include <string>
#include "../Math/Transform.hpp"
//////////////////////////////////////////////////////////////////////////
const uint INVALID_JOINT_INDEX = 0xffffffff;
//////////////////////////////////////////////////////////////////////////
class BinaryStream;
class Pose;
//////////////////////////////////////////////////////////////////////////
struct Joint
{
    Joint(){}
	Joint(const Joint& copy) : name(copy.name), parentName(copy.parentName), global_transform(copy.global_transform) {}
    std::string name;
	std::string parentName;
    Matrix4 global_transform;
};
//////////////////////////////////////////////////////////////////////////
class Skeleton
{
public:
	Skeleton();
	~Skeleton();
	

    void clear();

    void addJoint(const std::string& name, const std::string& parentName, const Matrix4& transform);

    const uint getJointCount() const;
	const std::string getJointName(uint index) const;
    
    uint getJointIndex(const std::string& name) const;
	uint getJointParentIndex(uint index) const;
	uint getJointParentIndex(const std::string& name) const;

    Matrix4 getJointTransform(uint index) const;
    Matrix4 getJointTransform(const std::string& name) const;

	//Skeletons are typically only used for reference and never changed.
	//This should only be called once, after skeleton creation, for cases where the bind locals are desired.
	void CreateLocalTransforms();
	void SetBindPose(const Pose& bindPose);
	SQT getLocalTransform(uint index) const;
	SQT getLocalTransform(const std::string& name) const;
	const bool localsAreValid() const;//Only checks if the number of locals is equal to the number of joints. Does not ensure they where created for this paticular skeleton.

	void write(BinaryStream &stream);
	void read(BinaryStream &stream);
public:
    std::vector<Joint> m_joints;
private:
	std::vector<SQT> m_local_joints;
};