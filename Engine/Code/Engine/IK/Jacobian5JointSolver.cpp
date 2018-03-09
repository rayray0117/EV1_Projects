#include "Engine/IK/Jacobian5JointSolver.hpp"
//#include "Engine/Math/Vector3.hpp"
#include "Engine/IK/IKChain.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Core/Profiler.hpp"

#include "Engine/General/UniversalFunction.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/Time.hpp"
//////////////////////////////////////////////////////////////////////////
constexpr float MaxAngleJtranspose = 30.f * EV1::DEG2RAD; //Value in degress * value to convert it to radians.
//////////////////////////////////////////////////////////////////////////
void Jacobian5JointSolver::Solve(IKChain* chain, const Vector3& goal)
{
	//PROFILE_SCOPE_FUNCTION();
	float beginTime = (float)GetCurrentTimeSeconds();

	JacobianMatrix.makeIdentity(); //Reset Jacobian for safety.
	//JacobianMatrix.values[15] = 0;
	uint countNum = 0;
	//Get the rotation axis of each changeable joint for when it's has 0 theta. May cause an error and might need to be done on initial binding phase.
	for (uint i = chain->getNumberOfLinks() - 1; i >= 1; --i)
	{
		chain->getLinksRotationAxisAndAngle(i, initialAxes[countNum], thetas[countNum]);
		++countNum;
	}

	uint currentIterations = 0;
	while (!isCloseEnough(chain, goal) && currentIterations < maxIterations)
	{
		++currentIterations;
		ComputeJacobian(chain, goal);
		CalcDeltaThetas();
		UpdateThetas(chain);
	}

	float endTime = (float)GetCurrentTimeSeconds();
	std::string str = Stringf("Jacobian ,%s, %i, %i, %f, %f, %s, %f \n",
		chain->m_name.c_str(), currentIterations, maxIterations, (endTime - beginTime) * 1000 * 1000, IK_TOLERANCE,
		vectorToString(goal).c_str(), calcDistance(goal, chain->getEndEffectorPosition_Global()));
	DebuggerPrintf(str.c_str());
	UF->CallF("IKData", (void*)str.c_str());
}
////////////////////////////////////////////////////////////////////////// Protected //////////////////////////////////////////////////////////////////////////
void Jacobian5JointSolver::ComputeJacobian(IKChain* chain, const Vector3& goal)
{
	//PROFILE_SCOPE_FUNCTION();
	dS = goal - chain->getEndEffectorPosition_Global();

	Vector3 temp;
	uint countNum = 1;
	for (uint i = chain->getNumberOfLinks() - 1; i >= 1 && countNum < 5; --i)
	{
		temp = chain->getEndEffectorPosition_Global() - chain->getLinksPosition_Global(i);
		temp = crossProduct(chain->getLinksRotationAxis(i), temp);
		AddVectorToJacobianMatrix(countNum, temp);
		++countNum;
	}
}

void Jacobian5JointSolver::CalcDeltaThetas()
{
	//PROFILE_SCOPE_FUNCTION();
	Matrix4 jTranspose = JacobianMatrix.getTranpose();
	deltaThetas = jTranspose.TransformVector(Vector4(dS, 0.f)); //Need to use 4x4 matrix as 3X4 matrix. Not sure if direction or position works.
	deltaThetas *= dampening;
	
	/*/
	Vector4 dTtemp = JacobianMatrix.TransformVector(deltaThetas);
	dT.setXYZ(dTtemp.x, dTtemp.y, dTtemp.z);  //w shouldn't matter
	deltaThetas *= dampening;
	//*/
}

void Jacobian5JointSolver::UpdateThetas(IKChain* chain)
{
	//PROFILE_SCOPE_FUNCTION();
	//Update theta values.
	thetas += deltaThetas;

	//Recalculate transforms.
	//Get local transforms so I can apply the change in the theta correctly.
	std::vector<SQT> localTrans;
	localTrans.reserve(chain->getNumberOfLinks());
	for (uint i = 0; i < chain->getNumberOfLinks(); ++i)
	{
		SQT localTran;// = chain->getLinksJoint(i)->local_transform;
			localTran.setFromMatrix(
		chain->getLinksJoint(i)->getGlobalTransform().getAsMatrix().getTransformed(
			chain->getLinksJoint(i)->parents_global_transform.getAsMatrix().getInverse()));



		if (localTran.scale.isMostlyEqualTo(Vector3::ONE))
			localTran.scale = Vector3::ONE; //Correct for float point error.
		localTrans.push_back(localTran);
	}

	uint countNum = 0;
	//Start from the root and work down to the end effector
	for (int i = chain->getNumberOfLinks() - 1; i >= 0; --i)
	{
		if (i > 0)
			localTrans[i].rotation = Quaternion(initialAxes[countNum], thetas[countNum]); //Rotate around initial axis by the theta.

		//chain->m_links[i]->SetFromLocalTransformForCCD(localTrans[i]);
		chain->getLinksJoint(i)->SetRotation_Global(localTrans[i].rotation);
		chain->UpdateAllParentTransforms();
		++countNum;
	}
}

void Jacobian5JointSolver::AddVectorToJacobianMatrix(uint i, const Vector3& temp)
{
	//PROFILE_SCOPE_FUNCTION();
	switch (i)
	{
	case 1: JacobianMatrix.setIBasis(temp); break;
	case 2: JacobianMatrix.setJBasis(temp); break;
	case 3: JacobianMatrix.setKBasis(temp); break;
	case 4: JacobianMatrix.setTranslation(temp); break;
	default: ERROR_AND_DIE("Tried to add a vector to an invalid case value!");
	}
}

////////////////////////////////////////////////////////////////////////// Private //////////////////////////////////////////////////////////////////////////
Vector3 Jacobian5JointSolver::GetDeltaOrientation(IKChain* chain, const Vector3& goal)
{
	//PROFILE_SCOPE_FUNCTION();
	Matrix4 Jt = GetJacobianTranspose(chain);
	Vector3 V = goal - chain->getEndEffectorPosition_Global();
	Vector3 result = Jt.TransformDirection(V);
	return result;
}

Matrix4 Jacobian5JointSolver::GetJacobianTranspose(IKChain* /*chain*/)
{
	//PROFILE_SCOPE_FUNCTION();
	return Matrix4();
}
