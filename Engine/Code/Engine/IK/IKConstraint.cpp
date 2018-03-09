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
