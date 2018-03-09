#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Core/BinaryStream.hpp"
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

void Skeleton::addJoint(const char* name, const char* parentName, const Matrix4& transform)
{
    Joint joint;
    joint.name = name;
	if(parentName)
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

uint Skeleton::getJointIndex(const char* name) const
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
	return getJointIndex(m_joints[index].parentName.c_str());
}

Matrix4 Skeleton::getJointTransform(uint index) const
{
	/*/
     if (!m_joints[index].parentName.empty())
     {
         Matrix4 parentTransform = getJointTransform(m_joints[index].parentName.c_str());
         return m_joints[index].global_transform.getTransformed(parentTransform);
     }
     else
     {
		 //*/
        return m_joints[index].global_transform;
    //}
}

Matrix4 Skeleton::getJointTransform(const char* name) const
{
    uint index = getJointIndex(name);
    return getJointTransform(index);
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

