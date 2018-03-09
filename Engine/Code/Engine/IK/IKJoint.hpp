#pragma once
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Math/Transform.hpp"
#include <string>
//////////////////////////////////////////////////////////////////////////
class IKConstraint;
//////////////////////////////////////////////////////////////////////////
struct IKJoint
{
	friend class IKChain;
	friend class IKLink;
public:
	IKJoint() {}
	IKJoint(const Joint& copy) : name(copy.name), parentName(copy.parentName)/*, global_transform(copy.global_transform)*/ {}
	IKJoint(const Joint& copy, Skeleton* o_skeleton);

	//void SetLocalFromParent(const Matrix4& parent_global);
	//void SetGlobalFromParent(const Matrix4& parent_global);

	void SetGlobalTransform(const SQT& global, const SQT& parentGlobal);
	void SetPosition_Global(const Vector3& position);
	void SetRotation(const Quaternion& rotation);
	void SetRotation_Global(const Quaternion& rotation);

	const SQT getGlobalTransform() const;
	const Vector3 getPosition_Global() const;
	Quaternion getRotation_Global() const;
public:
	std::string name;
	std::string parentName;
	SQT local_transform;
	IKConstraint* m_constraint = nullptr;
	
	SQT parents_global_transform; //This should only be used on roots!
private:
	//Matrix4 global_transform; //This should not be used outside of the functions already set!
};