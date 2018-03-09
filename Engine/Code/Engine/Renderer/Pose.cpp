#include "Engine/Renderer/Pose.hpp"
//////////////////////////////////////////////////////////////////////////
const Pose interpolate(const Pose& start, const Pose& end, float fractionToEnd)
{
	Pose result;
	result.m_transforms.resize(start.m_transforms.size());

	for (uint i = 0; i < result.m_transforms.size(); i++)
	{
		result.m_transforms[i] = interpolate(start.m_transforms[i], end.m_transforms[i], fractionToEnd);
	}

	return result;
}