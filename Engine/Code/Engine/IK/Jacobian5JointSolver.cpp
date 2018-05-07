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
#include "../Core/Command.hpp"
#include "IKConstraint.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "../Core/Rgba.hpp"
#include "../Renderer/BitmapFont.hpp"
//////////////////////////////////////////////////////////////////////////
constexpr float MaxAngleJtranspose = 30.f * EV1::DEG2RAD; //Value in degress * value to convert it to radians.
bool g_reset = true;

COMMAND(jacob_reset, "")
{
	g_reset = !g_reset;
}

struct jacobian_debugInfo
{
	Vector3 jointWorldPos;
	Vector3 dirToEnd;
	Vector3 dirToGoal;
	Vector3 rotAxis;
	float angle;
};

std::map<std::string, jacobian_debugInfo> g_jointInfos;
//////////////////////////////////////////////////////////////////////////
void Jacobian5JointSolver::Solve(IKChain* chain, const Vector3& goal)
{
	//PROFILE_SCOPE_FUNCTION();
	//float beginTime = (float)GetCurrentTimeSeconds();

	JacobianMatrix.makeIdentity(); //Reset Jacobian for safety.
	JacobianMatrix.values[15] = 0;
	//SetUpInitialAxes(chain, goal);
	//if (g_reset)
	//{
	//	chain->ResetToInitialAxes();
	//	chain->UpdateAllParentTransforms();
	//}
	uint currentIterations = 0;
	while (!isCloseEnough(chain, goal) && currentIterations < maxIterations)
	{
		SetUpInitialAxes(chain, goal);
		++currentIterations;
		ComputeJacobian(chain, goal);
		CalcDeltaThetas();
		UpdateThetas(chain);
	}

	//float endTime = (float)GetCurrentTimeSeconds();
	//std::string str = Stringf("Jacobian ,%s, %i, %i, %f, %f, %s, %f \n",
	//	chain->m_name.c_str(), currentIterations, maxIterations, (endTime - beginTime) * 1000 * 1000, IK_TOLERANCE,
	//	vectorToString(goal).c_str(), calcDistance(goal, chain->getEndEffectorPosition_Global()));
	////DebuggerPrintf(str.c_str());
	//UF->CallF("IKData", (void*)str.c_str());
}

void Jacobian5JointSolver::DebugRender() const
{
	g_mainEngineRenderer->ClearDepth();
	g_mainEngineRenderer->BindShader(DEFAULT_UNLIT_SHADER);
	g_mainEngineRenderer->BindTexture("White");
	g_mainEngineRenderer->BindTexture("FlatNormal", NORMAL_TEXTURE_INDEX);
	g_mainEngineRenderer->BindTexture("White", SPECULAR_TEXTURE_INDEX);
	g_mainEngineRenderer->BindTexture("White", 3);

	for (const auto& current : g_jointInfos)
	{
		Vector3 startpos = current.second.jointWorldPos;

		g_mainEngineRenderer->drawDebugTriDirectionalPlane(startpos, current.second.dirToEnd, current.second.dirToGoal, Rgba(Rgba::YELLOW,128));
		g_mainEngineRenderer->drawDebugTriDirectionalPlane(startpos, current.second.dirToGoal, current.second.dirToEnd, Rgba(90, 90, 0, 128));
		
		g_mainEngineRenderer->drawLineWithDirection(startpos, current.second.dirToEnd, Rgba::BLACK);
		g_mainEngineRenderer->drawLineWithDirection(startpos, current.second.dirToGoal, Rgba::BLACK);

		g_mainEngineRenderer->drawLineWithDirection(startpos, current.second.rotAxis, Rgba::RED, 10.f);
	}

	//Draw debug text
	g_mainEngineRenderer->SetOrthoProjection(Vector2::ZERO, Vector2(160, 90));
	g_mainEngineRenderer->EnableDepth(false);

	g_mainEngineRenderer->pushMatrix();
	{
		g_mainEngineRenderer->translate2D(Vector2::Yaxis(90.f));
		g_mainEngineRenderer->translate2D(-Vector2::YAXIS);
		std::string txtToDraw = Stringf("Jacobian Joint Rotational Axes");
		float txtWidth = g_mainEngineRenderer->CreateOrGetBitmapFont("SquirrelFixedFont")->getTextWidth(txtToDraw);
		g_mainEngineRenderer->drawAABB2(Vector2::ZERO, Vector2(txtWidth, 1.f), Rgba(Rgba::BLACK, .25f));
		g_mainEngineRenderer->DrawText2D(Vector2::ZERO, txtToDraw, 1.f, Rgba::WHITE);
	}
	for (const auto& current : g_jointInfos)
	{
		g_mainEngineRenderer->translate2D(-Vector2::YAXIS);
		std::string txtToDraw = Stringf("%s: %s   angle = %f", current.first.c_str(), vectorToString(current.second.rotAxis).c_str(), current.second.angle);
		float txtWidth = g_mainEngineRenderer->CreateOrGetBitmapFont("SquirrelFixedFont")->getTextWidth(txtToDraw);
		g_mainEngineRenderer->drawAABB2(Vector2::ZERO, Vector2(txtWidth, 1.f), Rgba(Rgba::BLACK, .25f));
		g_mainEngineRenderer->DrawText2D(Vector2::ZERO, txtToDraw, 1.f, Rgba::WHITE);
	}
	g_mainEngineRenderer->popMatrix();
}

////////////////////////////////////////////////////////////////////////// Protected //////////////////////////////////////////////////////////////////////////
void Jacobian5JointSolver::SetUpInitialAxes(IKChain* chain, const Vector3& goal)
{
	uint countNum = 0;
	//Get the rotation axis of each changeable joint for when it's has 0 theta. May cause an error and might need to be done on initial binding phase.
	for (uint i = chain->getNumberOfLinks() - 1; i >= 1; --i)
	{
		Vector3 currentJointPos = chain->getLinksPosition_Global(i);

		Vector3 dirToEnd = chain->getEndEffectorPosition_Global() - currentJointPos;
		Vector3 dirToGoal = goal - currentJointPos;
		//dirToEnd.normalize();
		//dirToGoal.normalize();
		Vector3 crossResult= crossProduct(dirToEnd, dirToGoal).getNormalizedVector();
		initialAxes[countNum] = crossResult;
		//DebuggerPrintf("For joint %s, found axis: %s, goal: %s, end: %s \n", vectorToString(initialAxes[countNum]).c_str(), vectorToString(dirToGoal).c_str(), vectorToString(dirToEnd).c_str());
		thetas[countNum] = 0;


		jacobian_debugInfo info;
		info.rotAxis = initialAxes[countNum];
		info.dirToEnd = dirToEnd;
		info.dirToGoal = dirToGoal;
		info.jointWorldPos = currentJointPos;
		g_jointInfos[chain->getLinksJoint(i)->name] = info;
		
		
		++countNum;
	}
}

void Jacobian5JointSolver::ComputeJacobian(IKChain* chain, const Vector3& goal)
{
	//PROFILE_SCOPE_FUNCTION();
	dS = goal - chain->getEndEffectorPosition_Global();
	dS *= dampening;

	Vector3 temp;
	uint countNum = 0;
	for (uint i = chain->getNumberOfLinks() - 1; i >= 1 && countNum < 5; --i)
	{
		temp = chain->getEndEffectorPosition_Global() - chain->getLinksPosition_Global(i);
		Vector3 rotAxis = initialAxes[countNum];//chain->getLinksRotationAxis(i);
		temp = crossProduct(rotAxis, temp) * EV1::DEG2RAD;
		AddVectorToJacobianMatrix(countNum+1, temp);
		++countNum;
	}
}

void Jacobian5JointSolver::CalcDeltaThetas()
{
	//PROFILE_SCOPE_FUNCTION();
	Matrix4 jTranspose = JacobianMatrix.getTranpose();
	deltaThetas = jTranspose.TransformVector(Vector4(dS, 1.f)); //Need to use 4x4 matrix as 3X4 matrix. Not sure if direction or position works.
	//deltaThetas *= dampening;
	//DebuggerPrintf("Delta Thetas: %f, %f, %f, %f", deltaThetas.x * EV1::RAD2DEG, deltaThetas.y * EV1::RAD2DEG, deltaThetas.z * EV1::RAD2DEG, deltaThetas.w * EV1::RAD2DEG);
}

void Jacobian5JointSolver::UpdateThetas(IKChain* chain)
{
	//PROFILE_SCOPE_FUNCTION();
	//Update theta values.
	thetas += deltaThetas;


	uint countNum = 0;
	//Start from the root and work down to the end effector
	for (int i = chain->getNumberOfLinks() - 1; i >= 0 && countNum < 5; --i)
	{
		if (i == 0) continue;


		
		Quaternion rot = chain->getLinksJoint(i)->getGlobalTransform().rotation;
		Matrix4 rotationMat = chain->getLinksJoint(i)->parents_global_transform.getInverse().rotation.getAsMatrix();
		Vector3 rotAxis = rotationMat.TransformDirection(initialAxes[countNum]);
		g_jointInfos[chain->getLinksJoint(i)->name].angle = thetas[countNum];
		rot = Quaternion(rotAxis, thetas[countNum]) * chain->getLinksJoint(i)->local_transform.rotation;

		chain->getLinksJoint(i)->SetRotation(rot);
		CheckConstraint(chain->getLinksJoint(i));

		++countNum;
	chain->UpdateAllParentTransforms();
	}
}

void Jacobian5JointSolver::CheckConstraint(IKJoint* jointToCheck)
{
	if (!m_simpleConstraint || jointToCheck->m_constraint == nullptr) return;

	IKConstraint& constraint = *jointToCheck->m_constraint;
	Quaternion& qrotation = jointToCheck->local_transform.rotation;
	Vector3 euler = qrotation.getEulerAngles();

	Vector3 clampedEuler = constraint.AngleClamp(euler);
	qrotation.SetFromEuler(clampedEuler);
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
//*/
void JacobianSetDOFSolver::SetUpInitialAxes(IKChain* chain, const Vector3& goal)
{
	uint countNum = 0;
	//Get the rotation axis of each changeable joint for when it's has 0 theta. May cause an error and might need to be done on initial binding phase.
	for (uint i = chain->getNumberOfLinks() - 1; i >= 1; --i)
	{
		//While not needed since handled in getLinksRotationAxis, I'm leaving currentJointPos, dirToEnd, and dirToGoal in for debug purposes.
		Vector3 currentJointPos = chain->getLinksPosition_Global(i);
		Vector3 dirToEnd = chain->getEndEffectorPosition_Global() - currentJointPos;
		Vector3 dirToGoal = goal - currentJointPos;

		Vector3 crossResult = chain->getLinksRotationAxis(i,goal);
		initialAxes[countNum] = crossResult;
		//DebuggerPrintf("For joint %s, found axis: %s, goal: %s, end: %s \n", vectorToString(initialAxes[countNum]).c_str(), vectorToString(dirToGoal).c_str(), vectorToString(dirToEnd).c_str());
		thetas[countNum] = 0;


		jacobian_debugInfo info;
		info.rotAxis = initialAxes[countNum];
		info.dirToEnd = dirToEnd;
		info.dirToGoal = dirToGoal;
		info.jointWorldPos = currentJointPos;
		g_jointInfos[chain->getLinksJoint(i)->name] = info;


		++countNum;
	}
}
//*/