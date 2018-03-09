#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Game/BlockInfo.hpp"
#include "Game/Block.hpp"
//////////////////////////////////////////////////////////////////////////
struct RaycastResult
{
	bool didImpact;
	Vector3 hitPosition;
	Vector3 direction;
	BlockInfo blockInfo;
	BlockFaces face;
	float length;
	float fraction;
	float lastFraction;
};