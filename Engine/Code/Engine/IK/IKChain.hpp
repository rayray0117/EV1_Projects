#pragma once
#include "Engine/IK/IKLink.hpp"

#include <vector>
//////////////////////////////////////////////////////////////////////////
class IKSolver;
extern Vector3 g_rotationalAxis;
struct XMLNode;
//////////////////////////////////////////////////////////////////////////
class IKChain
{
public:
	IKChain(bool forceRoot = false);
	~IKChain();
	
	void SolveToGoal(const Vector3& goal);

	void AddLink(const Joint& end, Skeleton* o_skeleton = nullptr);
	void AddConstraints(const XMLNode& chainNode);

	IKJoint* getRootJoint() const;
	IKJoint* getEndEffectorJoint() const;
	IKLink*	 getRootLink() const;
	IKLink*	 getEndEffectorLink() const;
	Vector3  getRootPosition_Global() const;
	Vector3  getEndEffectorPosition_Global() const;

	IKJoint* getLinksJoint(uint index) const;
	Vector3  getLinksPosition_Global(uint index) const;
	float    getLinksLength(uint index) const;
	//Matrix4  getLinksGlobalTransform(uint index) const;
	Vector3 getLinksRotationAxis(uint index, const Vector3& goal) const;
	void	 getLinksRotationAxisAndAngle(uint index, Vector3& out_axis, float& out_angle) const;

	void getLinksInitialAxes(uint index, Vector3 &out_axis, float &out_angle) const;
	void ResetToInitialAxes();

	IKJoint* getJoint(const std::string& name) const;
	Vector3	 getMidJointDirection() const; //Only used for TwoBoneIK, so assume 1 is the midjoint.

	Vector3 getComponentSpacePosition(uint rootBasis, uint desiredJoint) const;

	float getTotalLength() const;
	uint  getNumberOfLinks() const;

	void SetRootsParentTransform_Global(const SQT& parentTrans);
	void UpdateAllParentTransforms();

	void DebugRender() const;
	
	void Reset();
public:
	std::string m_name; //Give it a name so debugging and stuff is easier.
	std::vector<IKLink*> m_links; //end effector at front, root in back.
	// IKJoint root, endEffector.
	IKSolver* m_solver; //What solver to use for this chain.
	bool m_forceRoot; //if true motions will force the roots transform in someway! Intended for a character's pelvis.
};