#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/Transform.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
// IKChainPose is a pose for one specific chain.
//////////////////////////////////////////////////////////////////////////
class IKChainPose
{
public:
	IKChainPose(){}
	~IKChainPose(){}

	
public:
	SQT	m_rootTransform; //Transform for where the root starts at/should be for the animation.
	SQT m_goalTransform; //The desired transform for the end effector.
	std::string m_chainName;
	//Using direction and length to find the end effector allows me to easily change how far to extend and where to 
	//Vector3 m_direction; //Direction toward the end effector.
	//float	m_length; //Distance to end effector.

public:
	friend const IKChainPose interpolate(const IKChainPose& start, const IKChainPose& end, float fractionToEnd)
	{
		IKChainPose result;
		result.m_chainName = start.m_chainName;
		TODO("Change IKChainInterpolate to interpolate the whole SQT not just the position!");
		result.m_rootTransform = interpolate(start.m_rootTransform, end.m_rootTransform, fractionToEnd);
		result.m_goalTransform = interpolate(start.m_goalTransform, end.m_goalTransform, fractionToEnd);


		return result;
	}

};
//////////////////////////////////////////////////////////////////////////
class IKPose
{
public:
	IKPose() {}
	~IKPose() {}
	

	uint getNumChainPoses() const { return m_chainPoses.size(); }
public:
	std::vector<IKChainPose> m_chainPoses;

public:
	friend const IKPose interpolate(const IKPose& start, const IKPose& end, float fractionToEnd)
	{
		IKPose result;
		result.m_chainPoses.resize(start.m_chainPoses.size());

		for (uint i = 0; i < result.m_chainPoses.size(); i++)
		{
			result.m_chainPoses[i] = interpolate(start.m_chainPoses[i], end.m_chainPoses[i], fractionToEnd);
		}

		return result;
	}

};