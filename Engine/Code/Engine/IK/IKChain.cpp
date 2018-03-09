#include "Engine/IK/IKChain.hpp"

#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/IK/IKSolver.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Core/XMLParseHelper.h"
#include "../Core/ErrorWarningAssert.hpp"
#include "IKConstraint.hpp"
#include "../Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
const float a = sqrtf(14.0f) / 8.0f;
const float b = 1.0f / 8.0f;
const float c = 7.0f / 8.0f;
Vector3 g_rotationalAxis = /*Vector3::XAXIS;*/ Vector3(b, a, c);
//////////////////////////////////////////////////////////////////////////
IKChain::IKChain(bool forceRoot)
	: m_forceRoot(forceRoot)
{

}
IKChain::~IKChain()
{
	for (uint i = 0; i < m_links.size(); ++i)
	{
		SAFE_DELETE(m_links[i]);
	}
	m_links.clear();
}
//////////////////////////////////////////////////////////////////////////
void IKChain::SolveToGoal(const Vector3& goal)
{
	m_solver->Solve(this, goal);
}

void IKChain::AddLink(const Joint& end, Skeleton* o_skeleton)
{
	IKLink* newLink = new IKLink();

	if (o_skeleton == nullptr)
		newLink->m_joint = IKJoint(end);
	else
		newLink->m_joint = IKJoint(end, o_skeleton);
	
	newLink->m_length = 0.f;

	if (!m_links.empty())
	{
		IKLink* childLink = m_links.back();
		childLink->SetRoot(&newLink->m_joint);
	}
	m_links.push_back(newLink);
}

void IKChain::AddConstraints(const XMLNode& chainNode)
{
	//Ensure using the correct XMLNode
	{
		ASSERT_OR_DIE(m_name == ParseXMLAttribute(chainNode, "name", std::string("ERROR_NAME_NOT_SET")), "Tried to apply a constraint from xml to the wrong chain!");
	}

	int numConstraints = chainNode.nChildNode("Constraint");
	for (int i = 0; i < numConstraints; ++i)
	{
		XMLNode conNode = chainNode.getChildNode("Constraint", i);
		std::string jointName = "NO_JOINT_NAME";

		jointName = ParseXMLAttribute(conNode, "jointName", jointName);
		{
			ASSERT_OR_DIE(jointName != "NO_JOINT_NAME", C_STRF("ERROR: Constraint without a specific joint in chain %s", m_name.c_str()));
		}

		IKJoint* joint = getJoint(jointName);
		IKConstraint* constraint = new IKConstraint(conNode);
		joint->m_constraint = constraint;
	}
}

IKJoint* IKChain::getRootJoint() const
{
	IKLink* root = getRootLink();
	return &root->m_joint;
}

IKJoint* IKChain::getEndEffectorJoint() const
{
	IKLink* end = getEndEffectorLink();
	return &end->m_joint;
}

IKLink* IKChain::getRootLink() const
{
	return m_links.back();
}

IKLink* IKChain::getEndEffectorLink() const
{
	return m_links.front();
}

Vector3 IKChain::getRootPosition_Global() const
{
	IKJoint* root = getRootJoint();
	return root->getPosition_Global();
}

Vector3 IKChain::getEndEffectorPosition_Global() const
{
	IKJoint* end = getEndEffectorJoint();
	return end->getPosition_Global();
}
//////////////////////////////////////////////////////////////////////////
IKJoint* IKChain::getLinksJoint(uint index) const
{
	return &m_links[index]->m_joint;
}

Vector3 IKChain::getLinksPosition_Global(uint index) const
{
	IKJoint* joint = getLinksJoint(index);
	return joint->getPosition_Global();
}

float IKChain::getLinksLength(uint index) const
{
	return m_links[index]->m_length;
}

Vector3 IKChain::getLinksRotationAxis(uint index) const
{
	/*/
	IKJoint* joint = getLinksJoint(index);
	if (joint->m_constraint != nullptr)
	{
		Quaternion rotation(joint->getGlobalTransform().rotation);
		Vector3 rotAxis = rotation.RotateVector(joint->m_constraint->m_rotationalAxis);
		return rotAxis;
	}
	return g_rotationalAxis;
	/*
	Quaternion rotation(joint->getGlobalTransform().rotation);
	Vector3 out_axis;
	float out_angle;
	rotation.getAxisAndAngle(out_axis, out_angle);
	return out_axis;
	//*/
	return g_rotationalAxis;
}

void IKChain::getLinksRotationAxisAndAngle(uint index, Vector3& out_axis, float& out_angle) const
{
	/*/
	IKJoint* joint = getLinksJoint(index);
	if (joint->m_constraint != nullptr)
	{
		Quaternion rotation(joint->getGlobalTransform().rotation);
		Vector3 rotAxis = rotation.RotateVector(joint->m_constraint->m_rotationalAxis);
		out_axis = rotAxis;
		out_angle = 0;
		return;
	}
	Quaternion rotation(joint->getGlobalTransform().rotation);
	rotation.getAxisAndAngle(out_axis, out_angle);
	//*/
	out_axis = getLinksRotationAxis(index);
	out_angle = 0;
	//out_axis.getNormalizedVector();
}

IKJoint* IKChain::getJoint(const std::string& name) const
{
	for (uint i = 0; i < getNumberOfLinks(); ++i)
	{
		IKJoint* current = getLinksJoint(i);
		if (current->name == name)
		{
			return current;
		}
	}

	ERROR_RECOVERABLE(C_STRF("joint %s not found in chain %s", name.c_str(), m_name.c_str()));
	return nullptr;
}

Vector3 IKChain::getMidJointDirection() const
{
	IKJoint* midJoint = getLinksJoint(1);
	if (midJoint->m_constraint != nullptr)
	{
		return midJoint->m_constraint->m_direction;
	}
	return Vector3::YAXIS;
}

//////////////////////////////////////////////////////////////////////////
float IKChain::getTotalLength() const
{
	//Opitmization for the future. Doing this solution for now because didn't think of it initially and this is just simpler at the moment.
	//Could store this as a member variable like CALIKO FARBIK does. 
	//That would make getting the value constant (Big O of 1) and move the cost for getting it on the creation, which shouldn't be getting called much anyway.
	float total = 0; 
	for (IKLink* current : m_links)
	{
		total += current->m_length;
	}
	return total;
}

uint IKChain::getNumberOfLinks() const
{
	return m_links.size();
}
//////////////////////////////////////////////////////////////////////////
void IKChain::SetRootsParentTransform_Global(const SQT& parentTrans)
{
	getRootJoint()->parents_global_transform = parentTrans;
}

void IKChain::UpdateAllParentTransforms()
{
	for (int i = getNumberOfLinks()-1; i >= 0; --i) //Start at the root and work way down.
	{
		m_links[i]->UpdateJointsParentTransForm();
	}
}

//////////////////////////////////////////////////////////////////////////
void IKChain::DebugRender() const
{
	PROFILE_SCOPE_FUNCTION();
	g_mainEngineRenderer->drawLine(getRootPosition_Global(), getEndEffectorPosition_Global(), Rgba::SKY_MOONLIGHT, Rgba::PERIWINKLE);

	MeshBuilder mb;
	for (uint i = 0; i < m_links.size(); ++i)
	{
		mb.begin(RHIEnum::PRIMATIVE_LINE_STRIP, false);
		mb.addSphereOutline(getLinksPosition_Global(i), 1.f, Rgba::PERIWINKLE);
		mb.end();

		if (m_links[i]->m_root != nullptr)
			g_mainEngineRenderer->drawLine(getLinksPosition_Global(i), m_links[i]->m_root->getPosition_Global(), Rgba::YELLOW, Rgba::BLACK);
		else
		{
			g_mainEngineRenderer->drawLine(getLinksPosition_Global(i), m_links[i]->m_joint.parents_global_transform.position, Rgba::MAGENTA, Rgba::OLIVE);
		}
	}
	
	Mesh* mesh = g_mainEngineRenderer->CreateAndGetMeshStatic(&mb);
	g_mainEngineRenderer->drawMesh(mesh);
	SAFE_DELETE(mesh);
	//*/
	g_mainEngineRenderer->ClearDepth();
	mb.clear();
	mb.begin(RHIEnum::PRIMATIVE_LINE_STRIP, false);
	mb.addSphereOutline(Vector3::ZERO, 1.f, Rgba::FORESTGREEN);
	mb.end();
	mesh = g_mainEngineRenderer->CreateAndGetMeshStatic(&mb);
	for (uint i = 0; i < m_links.size(); ++i)
	{
		
		g_mainEngineRenderer->pushMatrix();
		if (m_links[i]->m_root != nullptr)
		{
			SQT rootTrans = getLinksJoint(i+1)->getGlobalTransform();
			SQT localTrans = getLinksJoint(i)->local_transform;
			SQT trans = localTrans.getCombined(rootTrans);
			g_mainEngineRenderer->transform(trans.getAsMatrix());
			g_mainEngineRenderer->drawLine(Vector3::ZERO, (Vector3::XAXIS*getLinksLength(i)), Rgba::FORESTGREEN, Rgba::NAVY_BLUE); //Because my length is from me to my root, make the length - and end with a Navy blue color
		}
		else
		{
			g_mainEngineRenderer->transform(getLinksJoint(i)->getGlobalTransform().getAsMatrix());
			g_mainEngineRenderer->drawLine(Vector3::ZERO, (Vector3::XAXIS * 10.f ), Rgba::ORANGE, Rgba::ORANGE); //Because my length is from me to my root, make the length - and end with a Navy blue color
		}
		g_mainEngineRenderer->drawMesh(mesh);
		g_mainEngineRenderer->popMatrix();
	}

	//*/
	SAFE_DELETE(mesh);
}
//////////////////////////////////////////////////////////////////////////
COMMAND(ik_set_rotationAxis, "Type a Vector3 with spaces x y z")
{
	std::vector<std::string> args;
	splitArgs(arguments, args);
	Vector3 inputAxis(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
	inputAxis.normalize();
	g_rotationalAxis = inputAxis;
}

float AlphaToFloat(const std::string& value)
{
	float result;
	if (value[1] == 'a')
		result = a;
	else if(value[1] == 'b')
	{
		result = b;
	}
	else
	{
		result = c;
	}

	if (value[0] == '-')
	{
		result = -result;
	}

	return result;
}

COMMAND(ik_abc, "Type exactly like so -a _b _c. Second character is always the letter to use, first is - or not")
{
	std::vector<std::string> args;
	splitArgs(arguments, args);

	float x, y, z;
	x = AlphaToFloat(args[0]);
	y = AlphaToFloat(args[1]);
	z = AlphaToFloat(args[2]);

	g_rotationalAxis = Vector3(x, y, z);
}