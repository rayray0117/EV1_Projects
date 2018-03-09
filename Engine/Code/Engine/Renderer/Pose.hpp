#pragma once
#include "Engine/Math/Transform.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class Pose
{
public:
	std::vector<SQT> m_transforms;

	friend const Pose interpolate(const Pose& start, const Pose& end, float fractionToEnd);
};