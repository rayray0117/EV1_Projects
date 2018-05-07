#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Core/BinaryStream.hpp"
#include "Pose.hpp"
//////////////////////////////////////////////////////////////////////////
Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{

}

void Skeleton::clear()
{
    m_joints.clear();
}

void Skeleton::addJoint(const std::string& name, const std::string& parentName, const Matrix4& transform)
{
    Joint joint;
    joint.name = name;
	if(!parentName.empty())
		joint.parentName = parentName;
    joint.global_transform = transform;
    m_joints.push_back(joint);
}

const uint Skeleton::getJointCount() const
{
    return m_joints.size();
}

const std::string Skeleton::getJointName(uint index) const
{
	return m_joints[index].name;
}

uint Skeleton::getJointIndex(const std::string& name) const
{
    for (uint i = 0; i < m_joints.size(); i++)
    {
        if (m_joints[i].name == name)
            return i;
    }
    return INVALID_JOINT_INDEX;
}

uint Skeleton::getJointParentIndex(uint index) const
{
	return getJointIndex(m_joints[index].parentName);
}

uint Skeleton::getJointParentIndex(const std::string& name) const
{
	uint index = getJointIndex(name);
	return getJointParentIndex(index);
}

Matrix4 Skeleton::getJointTransform(uint index) const
{
    return m_joints[index].global_transform;
}

Matrix4 Skeleton::getJointTransform(const std::string& name) const
{
    uint index = getJointIndex(name);
    return getJointTransform(index);
}

void Skeleton::CreateLocalTransforms()
{
	m_local_joints.resize(m_joints.size());
	for (uint i = 0; i < m_joints.size(); i++)
	{
		SQT LocalTrans;
		if (m_joints[i].parentName.empty()) //Means they are a root and are already in local space.
		{
			LocalTrans.setFromMatrix(m_joints[i].global_transform);
		}
		else
		{
			Matrix4 parent = getJointTransform(getJointParentIndex(i));
			Matrix4 me = getJointTransform(i);
			LocalTrans.setFromMatrix(me*parent.getInverse());
		}
		

		m_local_joints[i] = LocalTrans;
	}
}

void Skeleton::SetBindPose(const Pose& bindPose)
{
	for (uint i = 0; i < bindPose.m_transforms.size(); i++)
	{
		m_local_joints[i] = bindPose.m_transforms[i];
	}
}

SQT Skeleton::getLocalTransform(uint index) const
{
	return m_local_joints[index];
}

SQT Skeleton::getLocalTransform(const std::string& name) const
{
	uint index = getJointIndex(name);
	return getLocalTransform(index);
}

const bool Skeleton::localsAreValid() const
{
	return m_joints.size() == m_local_joints.size();
}

void Skeleton::write(BinaryStream &stream)
{
	uint jointSize = m_joints.size();
	stream.write(jointSize);
	for each(Joint var in m_joints)
	{
		stream.write(var);
	}
}

void Skeleton::read(BinaryStream &stream)
{
	m_joints.clear();
	uint jointSize;
	stream.read(jointSize);
	//m_joints.resize(jointSize);
	for(uint i =0; i< jointSize; i++ )
	{
		Joint var;
		stream.read(var);
		m_joints.push_back(var);
	}
}

