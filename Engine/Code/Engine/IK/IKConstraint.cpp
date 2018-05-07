#include "Engine/IK/IKConstraint.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Core/XMLParseHelper.h"
#include "../Core/ErrorWarningAssert.hpp"
//////////////////////////////////////////////////////////////////////////
const float a = sqrtf(14.0f) / 8.0f;
const float b = 1.0f / 8.0f;
const float c = 7.0f / 8.0f;
Vector3 defualtRotationAxis = Vector3(b, a, c);
//////////////////////////////////////////////////////////////////////////
FabrikJointType ParseFabJointType(const XMLNode& fabNode, const FabrikJointType& defaultValue)
{
	const std::string noType = "NO_FAB_JOINT_TYPE";
	std::string parsedValue = ParseXMLAttribute(fabNode, "type", noType);

	if (parsedValue == noType)
		return defaultValue;

	if (parsedValue == "ball")
		return F_BALL;
	if (parsedValue == "local_hinge")
		return F_L_HINGE;
	if (parsedValue == "global_hinge")
		return F_G_HINGE;
	if (parsedValue == "none" || parsedValue == "NONE")
		return F_NO_FCON;

	return defaultValue;

}
//////////////////////////////////////////////////////////////////////////
IKConstraint::IKConstraint()
	: m_rotationalAxis(defualtRotationAxis)
{

}

IKConstraint::IKConstraint(const XMLNode& conNode)
{
	int hasAngleCon = conNode.nChildNode("Angles");
	if (hasAngleCon > 0)
	{
		XMLNode angles = conNode.getChildNode("Angles");
		m_minAngles = ParseXMLAttribute(angles, "min", m_minAngles);
		m_maxAngles = ParseXMLAttribute(angles, "max", m_maxAngles);
		m_dampClamp = ParseXMLAttribute(angles, "clamp", 360.f);
		m_use360 = ParseXMLAttribute(angles, "360", m_use360);
	}
	int hasRotAxis = conNode.nChildNode("RotAxis");
	if (hasRotAxis > 0)
	{
		XMLNode axisNode = conNode.getChildNode("RotAxis");
		m_rotationalAxis = ParseXMLAttribute(axisNode, "axis", defualtRotationAxis);
	}

	int hasDir = conNode.nChildNode("Direction");
	if (hasDir > 0)
	{
		XMLNode axisNode = conNode.getChildNode("Direction");
		m_direction = ParseXMLAttribute(axisNode, "dir", m_direction);
		m_direction.normalize(); //Normalize for safety
	}

	int hasFABRIK = conNode.nChildNode("FABRIK");
	if (hasFABRIK > 0)
	{
		XMLNode fabNode = conNode.getChildNode("FABRIK");
		m_fab_type = ParseFabJointType(fabNode, m_fab_type);
		m_fab_ballConDegrees = ParseXMLAttribute(fabNode, "angle", m_fab_ballConDegrees);
		m_fab_CWConDegrees = ParseXMLAttribute(fabNode, "CW", m_fab_CWConDegrees);
		m_fab_CCWConDegrees = ParseXMLAttribute(fabNode, "CCW", m_fab_CCWConDegrees);
		fab_rotAxis = ParseXMLAttribute(fabNode, "axis", fab_rotAxis);
		fab_refAxis = ParseXMLAttribute(fabNode, "refAxis", fab_refAxis);
		fab_refAxis.normalize();
		fab_rotAxis.normalize();
	}
}

Vector3 IKConstraint::AngleClamp(const Vector3& euler)
{
	Vector3 result = euler;
	FixAngles(result);
	result.x = ClampToX(result.x);
	result.y = ClampToY(result.y);
	result.z = ClampToZ(result.z);
	return result;
}

void IKConstraint::FixAngles(Vector3& euler)
{
	if (m_use360.x > 0.f)
	{
		euler.x = FixAngleFor360(euler.x);
	}
	if (m_use360.y > 0.f)
	{
		euler.y = FixAngleFor360(euler.y);
	}
	if (m_use360.z > 0.f)
	{
		euler.z = FixAngleFor360(euler.z);
	}
}

//////////////////////////////////////////////////////////////////////////
float IKConstraint::ClampToX(float angle) const
{
	float result;
	result = clamp(angle, m_minAngles.x, m_maxAngles.x);
	return result;
}

float IKConstraint::FixAngleFor360(float angle) const
{
	float newAngle = angle;
	if (angle < 0.f)
	{
		newAngle = rangeMapFloat(angle, -180, 0, 180, 360);
		if (angle == -90.f)
			ASSERT_OR_DIE(newAngle == 270, "-90 did not turn into 270!");
	}	
	return newAngle;
}

float IKConstraint::ClampToY(float angle) const
{
	float result;
	result = clamp(angle, m_minAngles.y, m_maxAngles.y);
	return result;
}

float IKConstraint::ClampToZ(float angle) const
{
	float result;
	result = clamp(angle, m_minAngles.z, m_maxAngles.z);
	return result;
}
