#pragma once
#include "Engine/Math/Vector3.hpp"
//////////////////////////////////////////////////////////////////////////
struct XMLNode;
//////////////////////////////////////////////////////////////////////////
enum FabrikJointType
{
	F_BALL,
	F_G_HINGE,
	F_L_HINGE,
	F_NO_FCON,
	F_NUM_FAB_JOINT_TYPES
};
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
	Vector3 m_rotationalAxis = Vector3(-32203.f, -32203.f, -32203.f);
	Vector3 m_direction = Vector3::YAXIS;

	//Fabrik constraints
	FabrikJointType m_fab_type = F_BALL;
	float m_fab_ballConDegrees = 180; //For ball joints. 180 ~= no constraint
	float m_fab_CWConDegrees = 180; //For hinge joints,
	float m_fab_CCWConDegrees = 180; //For hinge joints,

	Vector3 fab_rotAxis = Vector3::XAXIS; //For hinge joints, axis a hinge may rotate
	Vector3 fab_refAxis = Vector3::XAXIS; //For hinge joints, axis used as point of reference for rotation
};