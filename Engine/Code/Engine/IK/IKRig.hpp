#pragma once
#include "Engine/IK/IKChain.hpp"

#include "Engine/Renderer/SkeletonInstance.hpp"
//////////////////////////////////////////////////////////////////////////
class IKMotion;
struct XMLNode;
//////////////////////////////////////////////////////////////////////////
class IKRig
{
public:
	IKRig();
	~IKRig();
	
	void DebugRender() const;

	IKChain* CreateAndAddChain(const std::string& name, uint root, uint end, bool forceRoot = false);
	IKChain* CreateAndAddChain(const std::string& name, const std::string& root, const std::string& end, bool forceRoot = false);
	IKChain* CreateAndAddChain(const XMLNode& node);
	
	IKChain* getChain(const std::string& name);

	void SolveChainToGoal(const std::string& chainName, const Vector3& goal);
	void SolveChainToGoal(const std::string& chainName, const Vector3& goal, const SQT& newRootGlobal);

	void ForcePose(); //Debug function to force the skeletonInst's pose to reflect the IKChains.
	void ForceChainsPose(const std::string& name);

	void ApplyMotion(const IKMotion* motion, float time);
	void ApplyMotion(const IKMotion* motion, uint frame);

	void AddFkChain(const std::string& root, const std::string& end);
	void AddFkChain(uint root, uint end);
	void ResetChains(); //Good for starting a new solver or clearing out bad rotation wonder.
	void TimedResetChains();//Good for clearing out bad rotation wonder.
public:
	//Possible case for a Hierarchy Container. Use in Engine 2.
	//Using a simple vector of chains to start off. Probably limits me to not be able to do multi chain end effectors.
	std::vector<IKChain*> m_chains;
	std::vector<std::string> m_fkJoints;
	Skeleton* mo_skeleton = nullptr;
	SkeletonInstance* mo_skeletonInst = nullptr;
	bool m_resetsAfterAnimLoop = true;
private:
	void ForceChainsPose(IKChain* chain);
};