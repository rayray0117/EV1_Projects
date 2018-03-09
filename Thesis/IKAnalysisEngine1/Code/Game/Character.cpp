#include "Game/Character.hpp"
#include "Engine/Renderer/Motion.hpp"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/IK/IKRig.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/IK/IKPose.hpp"
#include "Engine/IK/IKMotion.hpp"
#include "Game/Game.hpp"
#include "Engine/IK/IKChain.hpp"
//////////////////////////////////////////////////////////////////////////
const std::string ERROR_NO_ATTRIBUTE = "ERROR_BAD";
//////////////////////////////////////////////////////////////////////////
Character::Character(const XMLNode& element)
{
	m_name = ParseXMLAttribute(element, "name", m_name);

	//Get basic info stuff.
	{
		std::string modelPath = ParseXMLAttribute(element, "path", ERROR_NO_ATTRIBUTE);
		m_rig.LoadSkeletonMesh(modelPath);
		Motion* bindPose = m_rig.LoadMotion_Asset(modelPath, 1);
		bindPose->m_name = "Bind Pose";
		m_motions.push_back(bindPose);
	}

	// Get regular motions
	{
		int numMotions = element.nChildNode("Motion");
		for (int i = 0; i < numMotions; ++i)
		{
			XMLNode motionNode = element.getChildNode("Motion", i);
			std::string motionPath = ParseXMLAttribute(motionNode, "path", ERROR_NO_ATTRIBUTE);
			uint framerate = ParseXMLAttribute(motionNode, "framerate", 10);
			Motion* motion = m_rig.LoadMotion_Asset(motionPath, framerate);
			std::string name = ParseXMLAttribute(motionNode, "name", motion->m_name);
			motion->m_name = name;
			m_motions.push_back(motion);
		}
	}

	// Get IK info
	XMLNode IKNode = element.getChildNode("IK");
	{
		SAFE_DELETE(m_ikRig);
		m_ikRig = new IKRig();
		m_ikRig->mo_skeleton = m_rig.m_skeleton;
		m_ikRig->mo_skeletonInst = m_rig.m_skelInst;
		
		//Get and setup chains.
		int numChains = IKNode.nChildNode("Chain");
		for (int i = 0; i < numChains; ++i)
		{
			XMLNode chainNode = IKNode.getChildNode("Chain", i);
			m_ikRig->CreateAndAddChain(chainNode);
		}

		int numMotions = IKNode.nChildNode("Motion");
		m_ikMotions.reserve(numMotions);
		for (int i = 0; i < numMotions; ++i)
		{
			XMLNode motionNode = IKNode.getChildNode("Motion", i);
			std::string motionName = ParseXMLAttribute(motionNode, "name", ERROR_NO_ATTRIBUTE);
			IKMotion* ikmotion = new IKMotion(getMotion(motionName), m_ikRig);
			m_ikMotions.push_back(ikmotion);
		}
	}
}

Character::~Character()
{
	SAFE_DELETE(m_ikRig);
}
//////////////////////////////////////////////////////////////////////////
void Character::Update(float deltaSeconds)
{
	m_motionTime += deltaSeconds;
}

void Character::Render() const
{
	//Render basic_rig
	if (m_rig.isValid())
	{
		if (!m_useIKMotion)
			m_rig.m_skelInst->apply_motion(m_motions[m_motionIndex], m_motionTime);
		else if(m_motionIndex < m_ikMotions.size())
		{
			m_ikRig->ApplyMotion(m_ikMotions[m_motionIndex], m_motionTime);
			const IKPose currentPose = m_ikMotions[m_motionIndex]->getIKPose(m_motionTime);
			for (uint i = 0; i < currentPose.getNumChainPoses(); ++i)
			{
				IKChainPose chainPose = currentPose.m_chainPoses[i];
				g_end2->m_position = chainPose.m_goalTransform.position;
			}
		}

		g_renderer->pushMatrix();
		g_renderer->BindShader(m_rig.m_skinShader);
		g_renderer->drawSkinMeshSkel(*m_rig.m_skelInst, m_rig.m_mesh);

		if (g_theGame->m_debugMode)
		{
			g_renderer->ClearDepth();
			g_renderer->BindShader(DEFAULT_UNLIT_SHADER);
			g_renderer->BindTexture("White");
			g_renderer->BindTexture("FlatNormal", NORMAL_TEXTURE_INDEX);
			g_renderer->BindTexture("White", SPECULAR_TEXTURE_INDEX);
			g_renderer->BindTexture("White", 3);
			g_renderer->drawSkeletonInstance(*m_rig.m_skelInst, *m_rig.m_debug_skeleton);
		}

		g_renderer->popMatrix();
	}

	if (g_theGame->m_debugMode)
	{
		if (m_rig.isValid() && g_theGame->m_drawSkeleton)
		{
			g_renderer->ClearDepth();
			g_renderer->BindShader(DEFAULT_UNLIT_SHADER);
			g_renderer->BindTexture("White");
			g_renderer->BindTexture("FlatNormal", NORMAL_TEXTURE_INDEX);
			g_renderer->BindTexture("White", SPECULAR_TEXTURE_INDEX);
			g_renderer->BindTexture("White", 3);

			g_renderer->pushMatrix();
			g_renderer->drawMesh(m_rig.m_debug_skeleton);
			g_renderer->popMatrix();
		}
		if (m_ikRig != nullptr)
		{
			g_renderer->ClearDepth();
			g_renderer->pushMatrix();
			m_ikRig->DebugRender();
			g_renderer->drawSkeletonInstance(*m_rig.m_skelInst, *m_rig.m_debug_skeleton);
			g_renderer->popMatrix();
		}
		g_theGame->m_debugEndEffector.Render();

		if (g_end2)
			g_end2->Render();
	}
}

void Character::SetSolverForAll(IKSolver* solver)
{
	for (uint i = 0; i < m_ikRig->m_chains.size(); ++i)
	{
		m_ikRig->m_chains[i]->m_solver = solver;
	}
}

Motion* Character::getMotion(const std::string& name) const
{
	for (uint i = 0; i < m_motions.size(); ++i)
	{
		if (m_motions[i]->m_name == name)
			return m_motions[i];
	}

	return nullptr;
}
