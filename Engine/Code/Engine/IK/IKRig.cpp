#include "Engine/IK/IKRig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/IK/IKMotion.hpp"
#include "Engine/Core/Profiler.hpp"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Core/XMLParseHelper.h"
#include "../Core/StringUtils.hpp"
#include "FABRIKConstrainedSolver.hpp"
#include "Jacobian5JointSolver.hpp"
//////////////////////////////////////////////////////////////////////////
IKRig::IKRig()
{

}

IKRig::~IKRig()
{
	for (uint i = 0; i < m_chains.size(); i++)
	{
		SAFE_DELETE(m_chains[i]);
	}
}
//////////////////////////////////////////////////////////////////////////
void IKRig::DebugRender() const
{
	PROFILE_SCOPE_FUNCTION();
	for (uint i = 0; i < m_chains.size(); ++i)
	{
		m_chains[i]->DebugRender();
	}

	/*/
	for (IKChain* chain : m_chains)
	{
		
		for (uint i = 0; i < chain->getNumberOfLinks(); ++i)
		{
			IKLink* currentLink = chain->m_links[i];

			//Forcing the Pose of joint[i] to what was determined by the solver. Don't move this to a separate function yet, as it may need to be changed to account for a moving root once animation is added later!
			//Begin
			IKJoint& currentJoint = currentLink->m_joint;
			uint jointsIndex = mo_skeleton->getJointIndex(currentJoint.name.c_str());
			mo_skeletonInst->m_pose.m_transforms[jointsIndex] = currentJoint.local_transform;
			//End
		}
	}
	//*/
}

//////////////////////////////////////////////////////////////////////////
IKChain* IKRig::CreateAndAddChain(const std::string& name, uint root, uint end, bool forceRoot)
{
	ASSERT_OR_DIE(mo_skeleton != nullptr, "ERROR: There is no skeleton!");
	ASSERT_OR_DIE(root != end, "ERROR: Trying to make a chain with only one joint!");
	ASSERT_OR_DIE(mo_skeleton->m_joints.size() > end, "ERROR: end joint is out of skeleton's bounds!");

	IKChain* chain = new IKChain(forceRoot);
	chain->m_name = name;

	uint currentJointIndex = end;
	do 
	{
		ASSERT_OR_DIE(currentJointIndex != INVALID_JOINT_INDEX, "ERROR: Root joint was not found! Root needs to be a parent of the End Effector inorder to make a chain!");

		chain->AddLink(mo_skeleton->m_joints[currentJointIndex], mo_skeleton);
		uint parentIndex = mo_skeleton->getJointParentIndex(currentJointIndex);
		currentJointIndex = parentIndex;
	} while (currentJointIndex != root );
	chain->AddLink(mo_skeleton->m_joints[currentJointIndex], mo_skeleton);

	m_chains.push_back(chain);
	return chain;
}

IKChain* IKRig::CreateAndAddChain(const std::string& name, const std::string& root, const std::string& end, bool forceRoot)
{
	ASSERT_OR_DIE(mo_skeleton != nullptr, "ERROR: There is no skeleton!");
	uint rootIndex = mo_skeleton->getJointIndex(root.c_str());
	uint endIndex = mo_skeleton->getJointIndex(end.c_str());
	return CreateAndAddChain(name, rootIndex, endIndex, forceRoot);
}

IKChain* IKRig::CreateAndAddChain(const XMLNode& node)
{
	std::string chainName = "NO_NAME_SET";
	std::string root = "NO_ROOT_SET";
	std::string end = "NO_END_SET";
	bool forceRoot = false;

	chainName = ParseXMLAttribute(node, "name", chainName);
	root = ParseXMLAttribute(node, "root", root);
	end = ParseXMLAttribute(node, "end", end);
	forceRoot = ParseXMLAttribute(node, "forceRoot", forceRoot);

	//Insure no bad data was sent
	{
		ASSERT_OR_DIE(chainName != "NO_NAME_SET", "Did not set a name for an IK Chain!");
		ASSERT_OR_DIE(root != "NO_ROOT_SET", C_STRF("Did not set the root for IK Chain %s!", chainName.c_str()));
		ASSERT_OR_DIE(root != "NO_END_SET", C_STRF("Did not set the end effector for IK Chain %s!", chainName.c_str()));
	}

	IKChain* chain = CreateAndAddChain(chainName, root, end, forceRoot);
	chain->AddConstraints(node);
	return chain;
}

void IKRig::AddFkChain(const std::string& root, const std::string& end)
{
	ASSERT_OR_DIE(mo_skeleton != nullptr, "ERROR: There is no skeleton!");
	uint rootIndex = mo_skeleton->getJointIndex(root.c_str());
	uint endIndex = mo_skeleton->getJointIndex(end.c_str());
	AddFkChain(rootIndex, endIndex);
}


void IKRig::AddFkChain(uint root, uint end)
{
	ASSERT_OR_DIE(mo_skeleton != nullptr, "ERROR: There is no skeleton!");
	ASSERT_OR_DIE(root != end, "ERROR: Trying to make a chain with only one joint!");
	ASSERT_OR_DIE(mo_skeleton->m_joints.size() > end, "ERROR: end joint is out of skeleton's bounds!");

	uint currentJointIndex = end;
	do
	{
		ASSERT_OR_DIE(currentJointIndex != INVALID_JOINT_INDEX, "ERROR: Root joint was not found! Root needs to be a parent of the End Effector inorder to make a chain!");

		m_fkJoints.push_back(mo_skeleton->m_joints[currentJointIndex].name);
		uint parentIndex = mo_skeleton->getJointParentIndex(currentJointIndex);
		currentJointIndex = parentIndex;
	} while (currentJointIndex != root);
	m_fkJoints.push_back(mo_skeleton->m_joints[currentJointIndex].name);
}

void IKRig::ResetChains()
{
	for (IKChain* chain : m_chains)
	{
		chain->Reset();
	}
}

void IKRig::TimedResetChains()
{
	if (!m_resetsAfterAnimLoop) return;

	for (IKChain* chain : m_chains)
	{
		//Jacobian5JointSolver* wasAJacobian = dynamic_cast<Jacobian5JointSolver*>(chain->m_solver);
		//if (wasAJacobian != nullptr)
		//	continue; //JacobianSolver doesn't handle resets well.
		chain->Reset();
	}
}

IKChain* IKRig::getChain(const std::string& name)
{
	//PROFILE_SCOPE_FUNCTION();
	for (uint i = 0; i < m_chains.size(); ++i)
	{
		if (m_chains[i]->m_name == name)
			return m_chains[i];
	}

	return nullptr;
}
//////////////////////////////////////////////////////////////////////////
void IKRig::SolveChainToGoal(const std::string& chainName, const Vector3& goal)
{
	PROFILE_SCOPE_FUNCTION();
	IKChain* chain = getChain(chainName);
	if (chain == nullptr)
		return;

	FABRIKConstrainedSolver* isFabCon = dynamic_cast<FABRIKConstrainedSolver*>(chain->m_solver);
	if (isFabCon != nullptr)
	{
		isFabCon->m_rigCaller = this;
	}

	chain->SolveToGoal(goal);
}

void IKRig::SolveChainToGoal(const std::string& chainName, const Vector3& goal, const SQT& newRootGlobal)
{
	PROFILE_SCOPE_FUNCTION();
	IKChain* chain = getChain(chainName);
	IKJoint* joint = chain->getRootJoint();
	SQT	rootParentTrans;

	if (chain->m_forceRoot)
	{
		rootParentTrans = mo_skeletonInst->get_joint_global_transform(mo_skeleton->getJointIndex(joint->parentName.c_str()));
		joint->SetGlobalTransform(newRootGlobal, rootParentTrans);
		//joint->SetPosition_Global(newRootGlobal.position);
	}
	else
	{
		chain->SetRootsParentTransform_Global(mo_skeletonInst->get_joint_global_transform(mo_skeleton->getJointIndex(joint->parentName.c_str())));
	}
	chain->UpdateAllParentTransforms();
	FABRIKConstrainedSolver* isFabCon = dynamic_cast<FABRIKConstrainedSolver*>(chain->m_solver);
	if (isFabCon != nullptr)
	{
		isFabCon->m_rigCaller = this;
	}

	chain->SolveToGoal(goal);
}

void IKRig::ForcePose()
{
	PROFILE_SCOPE_FUNCTION();
	for (uint i = 0; i < m_chains.size(); ++i)
	{
		ForceChainsPose(m_chains[i]);
	}
}
////////////////////////////////////////////////////////////////////////// PRIVATE //////////////////////////////////////////////////////////////////////////
void IKRig::ForceChainsPose(IKChain* chain)
{
	PROFILE_SCOPE_FUNCTION();
	for (uint i = 0; i < chain->getNumberOfLinks(); ++i)
	{
		IKLink* currentLink = chain->m_links[i];

		//Forcing the Pose of joint[i] to what was determined by the solver. Don't move this to a separate function yet, as it may need to be changed to account for a moving root once animation is added later!
		//Begin
		IKJoint& currentJoint = currentLink->m_joint;
		mo_skeletonInst->SetJointsTransform(currentJoint.name, currentJoint.local_transform);
		//End
	}
}

void IKRig::ForceChainsPose(const std::string& name)
{
	PROFILE_SCOPE_FUNCTION();
	IKChain* chain = getChain(name);
	if (chain == nullptr)
		return;
	ForceChainsPose(chain);
}

void IKRig::ApplyMotion(const IKMotion* motion, float time)
{
	PROFILE_SCOPE_FUNCTION();
	if(nullptr != motion)
		motion->Evaluate(this, time);
}

void IKRig::ApplyMotion(const IKMotion* motion, uint frame)
{
	PROFILE_SCOPE_FUNCTION();
	if (nullptr != motion)
		motion->Evaluate(this, frame);
}

