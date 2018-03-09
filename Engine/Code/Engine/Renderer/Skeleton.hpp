#pragma once
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Core/EngineBase.hpp"
#include <vector>
#include <string>
//////////////////////////////////////////////////////////////////////////
const uint INVALID_JOINT_INDEX = 0xffffffff;
//////////////////////////////////////////////////////////////////////////
class BinaryStream;
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

    void addJoint(const char* name, const char* parentName, const Matrix4& transform);

    const uint getJointCount() const;
	const std::string getJointName(uint index) const;
    
    uint getJointIndex(const char* name) const;
	uint getJointParentIndex(uint index) const;

    Matrix4 getJointTransform(uint index) const;
    Matrix4 getJointTransform(const char* name) const;


	void write(BinaryStream &stream);
	void read(BinaryStream &stream);
public:
    std::vector<Joint> m_joints;
};