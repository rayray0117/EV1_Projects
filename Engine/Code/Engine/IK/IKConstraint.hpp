#pragma once
#include "Engine/Math/Vector3.hpp"
//////////////////////////////////////////////////////////////////////////
struct XMLNode;
//////////////////////////////////////////////////////////////////////////
class IKConstraint
{
public:
	IKConstraint();
	IKConstraint(const XMLNode& conNode);
	~IKConstraint(){}
	
	Vector3 AngleClamp(const Vector3& euler);

	void FixAngles(Vector3& euler);
	float FixAngleFor360(float angle) const;

	float ClampToX(float angle) const;
	float ClampToY(float angle) const;
	float ClampToZ(float angle) const;

public:
	Vector3 m_minAngles;
	Vector3 m_maxAngles;
	Vector3 m_use360 = Vector3::ZERO;
	float m_dampClamp;
	Vector3 m_rotationalAxis;
	Vector3 m_direction = Vector3::YAXIS;
};