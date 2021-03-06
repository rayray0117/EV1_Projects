#include "Engine/IK/IKMotion.hpp"
#include "Engine/IK/IKRig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Renderer/Motion.hpp"
#include "../Core/Time.hpp"
#include "../General/UniversalFunction.hpp"
//////////////////////////////////////////////////////////////////////////
IKMotion::IKMotion(Motion* keyFrameMotion, IKRig* rig)
{
	m_framerate = keyFrameMotion->m_framerate;
	SetDuration(keyFrameMotion->get_duration());
	m_name = keyFrameMotion->m_name;

	Pose initialPose = rig->mo_skeletonInst->m_pose;

	for (uint framIndex = 0; framIndex < keyFrameMotion->m_poses.size(); ++framIndex)
	{
		rig->mo_skeletonInst->m_pose = keyFrameMotion->m_poses[framIndex];
		IKPose pose;
		for (uint i = 0; i < rig->m_chains.size(); ++i)
		{
			IKChainPose poseLeg;
			poseLeg.m_chainName = rig->m_chains[i]->m_name;
			poseLeg.m_goalTransform = rig->mo_skeletonInst->get_joint_global_transform(rig->mo_skeleton->getJointIndex(rig->m_chains[i]->getEndEffectorJoint()->name.c_str()));
			if(rig->m_chains[i]->m_forceRoot)
				poseLeg.m_rootTransform = rig->mo_skeletonInst->get_joint_global_transform(rig->mo_skeleton->getJointIndex(rig->m_chains[i]->getRootJoint()->name.c_str()));
			else
				poseLeg.m_rootTransform = rig->mo_skeletonInst->get_joint_global_transform(rig->mo_skeleton->getJointIndex(rig->m_chains[i]->getRootJoint()->parentName.c_str()));

			pose.m_chainPoses.push_back(poseLeg);
		}
		for (uint k = 0; k < rig->m_fkJoints.size(); ++k)
		{
			pose.m_FK[rig->m_fkJoints[k]] = (rig->mo_skeletonInst->get_joint_local_transform(rig->m_fkJoints[k]));
		}

		m_ikPoses[framIndex] = pose;
	}

	rig->mo_skeletonInst->m_pose = initialPose; //Reset the pose.
}

//////////////////////////////////////////////////////////////////////////
float IKMotion::SetDuration(float time)
{
	uint numPoses = (uint)std::ceil(m_framerate * time) + 1;

	m_ikPoses.resize(numPoses);

	m_duration = time;
	return (float)numPoses;
}
//////////////////////////////////////////////////////////////////////////
IKPose IKMotion::getIKPose(float time) const
{
	PROFILE_SCOPE_FUNCTION();
	float frame = m_framerate * time;

	uint beginFrame = (uint)floor(frame);
	//*/
	uint endFrame = beginFrame + 1;
	float fracToEnd = frame - (float)beginFrame;
	//*/
	return interpolate(getIKPose(beginFrame), getIKPose(endFrame), fracToEnd);
	//return getIKPose(beginFrame);
}

IKPose IKMotion::getIKPose(uint frame_index) const
{
	PROFILE_SCOPE_FUNCTION();
	if (frame_index >= getFrameCount() - 1)
	{
		switch (m_mode)
		{
		case Motion::PLAY_ONCE: frame_index = getFrameCount() - 1;
			break;
		case Motion::PLAY_LOOP: frame_index = frame_index % (getFrameCount() - 1); //The -1 prevents doubled up frames for the end to beginning.
			break;
		case Motion::NUM_PLAY_MODES:
		default: ERROR_AND_DIE(Stringf("Playback Mode not set for motion %s", m_name.c_str()).c_str());
			break;
		}
	}

	return m_ikPoses[frame_index];
}

uint IKMotion::getFrameIndexNumber(uint frame_index) const
{
	if (frame_index >= getFrameCount() - 1)
	{
		switch (m_mode)
		{
		case Motion::PLAY_ONCE: frame_index = getFrameCount() - 1;
			break;
		case Motion::PLAY_LOOP: frame_index = frame_index % (getFrameCount() - 1); //The -1 prevents doubled up frames for the end to beginning.
			break;
		case Motion::NUM_PLAY_MODES:
		default: ERROR_AND_DIE(Stringf("Playback Mode not set for motion %s", m_name.c_str()).c_str());
			break;
		}
	}

	return frame_index;
}

//////////////////////////////////////////////////////////////////////////
void IKMotion::Evaluate(IKRig* rig, float time) const
{
	PROFILE_SCOPE_FUNCTION();
	const IKPose currentPose = getIKPose(time);
	timeSinceReset += time;
	if(timeSinceReset >= getDuration())
		rig->TimedResetChains();//Help Clear out wonder.
	Eval(currentPose, rig);
}

void IKMotion::Evaluate(IKRig* rig, uint frame) const
{
	const IKPose currentPose = getIKPose(frame);
	if (getFrameIndexNumber(frame) == 0)
		rig->TimedResetChains();//Help Clear out wonder.
	
	double beginTime = GetCurrentTimeSeconds();
	Eval(currentPose, rig);

	double endTime = GetCurrentTimeSeconds();
	double totalSolveTime = (endTime - beginTime) * 1000.0 /* * 1000.0*/;
	UF->CallF("IKFrameTest", (void*)C_STRF("%f", totalSolveTime));
}

void IKMotion::Eval(const IKPose &currentPose, IKRig* rig) const
{
	for (auto currentFK : currentPose.m_FK)
	{
		rig->mo_skeletonInst->SetJointsTransform(currentFK.first, currentFK.second);
	}

	for (uint i = 0; i < currentPose.getNumChainPoses(); ++i)
	{
		const IKChainPose& chainPose = currentPose.m_chainPoses[i];
		rig->SolveChainToGoal(chainPose.m_chainName, chainPose.m_goalTransform.position, chainPose.m_rootTransform);
		
		//If our end effector was FK then lets set it encase the solver messed it up.
		IKJoint* endJoint = rig->getChain(chainPose.m_chainName)->getEndEffectorJoint();
		std::string endName = endJoint->name;
		SQT endFk;
		if (currentPose.findTransform(endName, endFk))
		{
			endJoint->local_transform = endFk;
		}

		rig->ForceChainsPose(chainPose.m_chainName);
	}
}
