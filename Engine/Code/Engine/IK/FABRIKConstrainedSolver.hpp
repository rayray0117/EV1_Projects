#pragma once
#include "Engine/IK/FABRIKSolver.hpp"
#include "Engine/Math/Vector3.hpp"
//////////////////////////////////////////////////////////////////////////
class IKConstraint;
class Matrix4;
class IKRig;
//////////////////////////////////////////////////////////////////////////

const float min_con_angle_degrees = 0.f;
const float max_con_angle_degrees = 180.f;
//////////////////////////////////////////////////////////////////////////
enum JointType
{
	BALL,
	G_HINGE,
	L_HINGE,
	NO_FCON,
	NUM_FAB_JOINT_TYPES
};

struct FabrikJointHelper
{
	float rotorConDegrees = 2;//max_con_angle_degrees; //For ball joints
	float hingeClockwiseConDegrees = max_con_angle_degrees; //For hinge joints,
	float hingeCounterClockwiseConDegrees = max_con_angle_degrees; //For hinge joints,
	
	Vector3 rotAxis = Vector3::XAXIS; //For hinge joints, axis a hinge may rotate
	Vector3 refernceAxisUVDir; //For hinge joints, axis used as point of reference for rotation
	JointType type = NO_FCON;
};

struct FabrikBoneHelper
{
	Vector3 startPos;
	Vector3 endPos;
	float length; //Calc in constructor?
	FabrikJointHelper joint;
	//Vector3 relative_HingeRotAxis; //Gets updated at before the 2 steps.
	std::string m_name = ""; //Used for debugging only. Should be name of the two connecting joints

	Vector3 getDirectionUV();
};
//////////////////////////////////////////////////////////////////////////
class FABRIKConstrainedSolver : public FABRIKSolver
{
public:
	FABRIKConstrainedSolver() { /*maxIterations = 1;*/ }
	~FABRIKConstrainedSolver() {}
	

	virtual void Initialize(IKChain* chain) override;

	
	virtual void Solve(IKChain* chain, const Vector3& goal);
	virtual void ApplyTransforms(IKChain* chain) override;

	IKRig* m_rigCaller = nullptr;//Rig needs to trust the solver so the solver can do stuff.
protected:

	virtual void Backward(IKChain* chain, const Vector3& goal) override;
	virtual void Forward(IKChain* chain, const Vector3& goal) override;
	const Vector3 Constrain(const Vector3& curToNewPosDisp, const Vector3& direction, const Matrix4& mat, IKConstraint* constraint) const;
	//virtual bool isCloseEnough(IKChain* /*chain*/, const Vector3& /*goal*/, float /*epsilon = 3.0f*/) const override { return false; }

private:
	std::vector<FabrikBoneHelper> m_fakeChain;
	Vector3 BaseboneConstraintUV;
	Vector3 BaseboneRelativeConstaintUV;
};