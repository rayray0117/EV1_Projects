#include "Engine/IK/FABRIKConstrainedSolver.hpp"

#include "Engine/IK/IKChain.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/General/UniversalFunction.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "IKConstraint.hpp"
#include "Engine/IK/IKRig.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/SkeletonInstance.hpp"
//////////////////////////////////////////////////////////////////////////
Matrix4 createRotationMatrix(const Vector3& refDirection)
{
	Vector3 xaxis;
	Vector3 yaxis;
	Vector3 zaxis = refDirection;

	if (refDirection.z < -.999999f)
	{
		xaxis = Vector3::XAXIS;
		yaxis = Vector3::YAXIS;
	}
	else
	{
		float a = inverse_f(1.f + zaxis.z);
		float b = -zaxis.x * zaxis.y *a;
		xaxis = Vector3(1.0f - zaxis.x * zaxis.x * a, b, -zaxis.x).getNormalizedVector();
		yaxis = Vector3(b, 1.0f - zaxis.y * zaxis.y * a, -zaxis.y).getNormalizedVector();
	}

	return Matrix4(xaxis, yaxis, zaxis);
}

void SetupConstraints(FabrikBoneHelper& bone, IKJoint* realjoint)
{
	IKConstraint* constraint = realjoint->m_constraint;
	if (constraint == nullptr)
		return;


	bone.joint.rotorConDegrees = constraint->m_fab_ballConDegrees;
	bone.joint.hingeClockwiseConDegrees = constraint->m_fab_CWConDegrees;
	bone.joint.hingeCounterClockwiseConDegrees = constraint->m_fab_CCWConDegrees;
	bone.joint.rotAxis = constraint->fab_rotAxis;
	bone.joint.refernceAxisUVDir = constraint->fab_refAxis;

	FabrikJointType conType = constraint->m_fab_type;
	switch (conType)
	{
	case F_BALL: bone.joint.type = BALL;
		break;
	case F_G_HINGE: bone.joint.type = G_HINGE;
		break;
	case F_L_HINGE: bone.joint.type = L_HINGE;
		bone.joint.rotAxis = realjoint->getGlobalTransform().rotation.getAsMatrix().TransformDirection(bone.joint.rotAxis);
		break;
	case F_NO_FCON: bone.joint.type = NO_FCON;
	case F_NUM_FAB_JOINT_TYPES:
	default: bone.joint.type = BALL;
		break;
	}
}

std::string getDebugNameForSingleJoint(IKChain* chain, uint i)
{
	if (i == 0)
		return "end";
	else if (i == chain->getNumberOfLinks() - 1)
		return "root";

	return chain->getLinksJoint(i)->name;
}
//////////////////////////////////////////////////////////////////////////
void FABRIKConstrainedSolver::Initialize(IKChain* chain)
{
	FABRIKSolver::Initialize(chain);
	m_fakeChain.resize(chain->getNumberOfLinks()-1); // num links - 1 because end effector is stored on the end pos of the last bone
	for (int i = m_fakeChain.size() - 1; i >= 0; --i)
	{
		m_fakeChain[i].startPos = chain->getLinksPosition_Global(i+1);
		m_fakeChain[i].endPos = chain->getLinksPosition_Global(i);
		m_fakeChain[i].length = chain->getLinksLength(i);

		SetupConstraints(m_fakeChain[i], chain->getLinksJoint(i + 1));
		m_fakeChain[i].m_name = getDebugNameForSingleJoint(chain, i+1) + "_to_" + getDebugNameForSingleJoint(chain, i);
	}

	IKJoint* root = chain->getRootJoint();
	Vector3 dir = getDirection(root->parents_global_transform.position, root->getPosition_Global());
	BaseboneConstraintUV = m_fakeChain.back().getDirectionUV();
	BaseboneRelativeConstaintUV = m_fakeChain.back().joint.refernceAxisUVDir;
}

void FABRIKConstrainedSolver::Solve(IKChain* chain, const Vector3& goal)
{
	//std::vector<SQT> transforms;
	//Skeleton* mo_skeleton = m_rigCaller->mo_skeleton;
	//SkeletonInstance* mo_skelInst = m_rigCaller->mo_skeletonInst;
	//
	//for (uint i = 0; i < chain->getNumberOfLinks(); i++)
	//{
	//	SQT trans;
	//	trans.setFromMatrix(mo_skeleton->getJointTransform(chain->getLinksJoint(i)->name));
	//	transforms.push_back(trans);
	//}
	chain->Reset();


	Vector3 dir_RtoE;
	Vector3 dir_RtoG;
	IKJoint* rootJ = chain->getRootJoint();
	{
		Vector3 rootPos = chain->getRootPosition_Global();
		Vector3 endPos = chain->getEndEffectorPosition_Global();
		//Get displacement cause we'll transform them as directions. So no sqrt!
		dir_RtoG = getDirection(rootPos, goal);
		dir_RtoE = getDirection(rootPos, endPos);


		Matrix4 rotMat = rootJ->getGlobalTransform().getInverse().rotation.getAsMatrix();
		dir_RtoE = rotMat.TransformDirection(dir_RtoE);
		dir_RtoG = rotMat.TransformDirection(dir_RtoG);
	}


	
	float cosAngle = dotProduct(dir_RtoE, dir_RtoG);
	if ((cosAngle < /*1.f*/ 0.9999f))
	//if (dir_RtoE != Vector3::ZERO && !dir_RtoE.isMostlyEqualTo(dir_RtoG))
	{
		
		Vector3 rotationalAxis = crossProduct(dir_RtoE, dir_RtoG);
		rotationalAxis.normalize();

		float turnAngle = EV1::RAD2DEG * acosf(cosAngle);

		//Perform Rotation
		Quaternion rotateBy(rotationalAxis, turnAngle);
		rotateBy.Normalize();
		rotateBy = rootJ->local_transform.rotation * rotateBy;
		//parentGlobalTrans.rotation.Normalize();
		rootJ->SetRotation(rotateBy);
		chain->UpdateAllParentTransforms();
	}
	//if (dir_RtoE != Vector3::ZERO && !dir_RtoE.isMostlyEqualTo(dir_RtoG))
	//{
	//	Quaternion DeltaRotation = Quaternion::FindBetweenNormals(dir_RtoE, dir_RtoG);
	//	if(DeltaRotation != Quaternion())
	//	{
	//		DeltaRotation.Normalize();
	//		rootJ->SetRotation(rootJ->local_transform.rotation * DeltaRotation);
	//		//rootJ->local_transform.rotation.Normalize();
	//		//chain->UpdateAllParentTransforms();
	//	}
	//}

	FABRIKSolver::Solve(chain, goal);
}

void FABRIKConstrainedSolver::ApplyTransforms(IKChain* chain)
{
	m_globalPositions.front() = m_fakeChain.front().endPos;
	for (int i = m_fakeChain.size() - 1; i >= 0; --i)
	{
		m_globalPositions[i+1] = m_fakeChain[i].startPos;
	}

	FABRIKSolver::ApplyTransforms(chain);
}

//////////////////////////////////////////////////////////////////////////
//*/
void FABRIKConstrainedSolver::Backward(IKChain* /*chain*/, const Vector3& goal)
{
	//*/
	//Setup the end effector
	{
		FabrikBoneHelper& thisBone = m_fakeChain[0];
		thisBone.endPos = goal;
		Vector3 OuterToInnerDirUV = -thisBone.getDirectionUV();

		switch (thisBone.joint.type) //In this case this would be the joint that is the direct parent of the end effector. Ex: knee joint for a foot end effector.
		{
		case L_HINGE:
		case G_HINGE:
			OuterToInnerDirUV = OuterToInnerDirUV.projectOntoPlane(thisBone.joint.rotAxis);
			break;
		//case L_HINGE:
		//{
		//	Vector3 relativeHingeRotAxis = createRotationMatrix(m_fakeChain[1].getDirectionUV()).TransformDirection(thisBone.joint.rotAxis);
		//	//Vector3 relativeHingeRotAxis = crossProduct(m_fakeChain[1].getDirectionUV(), getDirection(m_fakeChain[1].startPos, m_fakeChain[0].endPos));
		//	OuterToInnerDirUV = OuterToInnerDirUV.projectOntoPlane(relativeHingeRotAxis);
		//}
			break;
		default:
			break;
		}

		Vector3 newStartLocation = goal + OuterToInnerDirUV * thisBone.length;
		thisBone.startPos = newStartLocation;
		if(m_fakeChain.size() > 1)
			m_fakeChain[1].endPos = newStartLocation; //Set the endPos for the parent bone to my new start pos
	}




	//Continue with the rest.
	for (uint i = 1; i < m_fakeChain.size(); ++i)
	{
		FabrikBoneHelper& thisBone = m_fakeChain[i];

		Vector3 OuterBoneOuterToInnerUV = -m_fakeChain[i - 1].getDirectionUV();
		Vector3 thisBoneOuterToInnerUV = -thisBone.getDirectionUV();

		if (thisBone.joint.type == BALL)
		{
			float angleBetweenDegs = Vector3::getAngleBetweenDegrees(OuterBoneOuterToInnerUV, thisBoneOuterToInnerUV);
			float constraintAngleDegs = thisBone.joint.rotorConDegrees;
			if (angleBetweenDegs > constraintAngleDegs)
			{
				thisBoneOuterToInnerUV = Vector3::getAngleLimitedUnitVectorDegrees(thisBoneOuterToInnerUV, OuterBoneOuterToInnerUV, constraintAngleDegs);
			}
		}
		else if (thisBone.joint.type == G_HINGE || thisBone.joint.type == L_HINGE)
		{
			thisBoneOuterToInnerUV = thisBoneOuterToInnerUV.projectOntoPlane(thisBone.joint.rotAxis);
		}
		//else if (thisBone.joint.type == L_HINGE)
		//{
		//	Matrix4 m;
		//	Vector3 relativeHingeRotationAxis;
		//	if (i < m_fakeChain.size() - 1)
		//	{
		//		m = createRotationMatrix(m_fakeChain[i + 1].getDirectionUV());
		//		relativeHingeRotationAxis = m.TransformDirection(thisBone.joint.rotAxis);
		//		relativeHingeRotationAxis = crossProduct(m_fakeChain[i + 1].getDirectionUV(), getDirection(m_fakeChain[i + 1].startPos, m_fakeChain[i].endPos));
		//	}
		//	else
		//	{
		//		relativeHingeRotationAxis = BaseboneRelativeConstaintUV;
		//	}
		//
		//	thisBoneOuterToInnerUV = thisBoneOuterToInnerUV.projectOntoPlane(relativeHingeRotationAxis);
		//}

		Vector3 newStartLocation = thisBone.endPos + thisBoneOuterToInnerUV * thisBone.length;
		thisBone.startPos = newStartLocation;
		if (i < m_fakeChain.size() - 1)
		{
			m_fakeChain[i + 1].endPos = newStartLocation; //Set the endPos for the parent bone to my new start pos
		}

		//ASSERT_RECOVERABLE(calcDistance(thisBone.startPos, thisBone.endPos) == thisBone.length, C_STRF("Error: bone did not maintain length. calc = %f, real = %f", calcDistance(thisBone.startPos, thisBone.endPos), thisBone.length));
	}
	//*/
}
//*/
//////////////////////////////////////////////////////////////////////////
void FABRIKConstrainedSolver::Forward(IKChain* chain, const Vector3& /*goal*/)
{
	//return;
	//*/
	for (int i = m_fakeChain.size() - 1; i >= 0; --i)
	{
		FabrikBoneHelper& thisBone = m_fakeChain[i];

		//if not the root/base bone
		if (i != (int)m_fakeChain.size() - 1)
		{
			Vector3 thisBoneInnerToOuterUV = thisBone.getDirectionUV();
			Vector3 prevBoneInnerToOuterUV = m_fakeChain[i + 1].getDirectionUV();
			if (thisBone.joint.type == BALL)
			{
				float angleBetweenDegs = Vector3::getAngleBetweenDegrees(prevBoneInnerToOuterUV, thisBoneInnerToOuterUV);
				float constraintAngleDegs = thisBone.joint.rotorConDegrees;

				if (angleBetweenDegs > constraintAngleDegs)
				{
					thisBoneInnerToOuterUV = Vector3::getAngleLimitedUnitVectorDegrees(thisBoneInnerToOuterUV, prevBoneInnerToOuterUV, constraintAngleDegs);
				}
			}
			else if (thisBone.joint.type == G_HINGE || thisBone.joint.type == L_HINGE)
			{
				// Get the hinge rotation axis and project our inner-to-outer UV onto it
				Vector3 hingeRotationAxis = thisBone.joint.rotAxis;
				thisBoneInnerToOuterUV = thisBoneInnerToOuterUV.projectOntoPlane(hingeRotationAxis);

				// If there are joint constraints, then we must honour them...
				float cwConstraintDegs = -thisBone.joint.hingeClockwiseConDegrees;
				float acwConstraintDegs = thisBone.joint.hingeCounterClockwiseConDegrees;
				if (!(areMostlyEqual(cwConstraintDegs, -max_con_angle_degrees, 0.001f)) && !(areMostlyEqual(acwConstraintDegs, max_con_angle_degrees, 0.001f)))
				{
					Vector3 hingeReferenceAxis = thisBone.joint.rotAxis;

					// Get the signed angle (about the hinge rotation axis) between the hinge reference axis and the hinge-rotation aligned bone UV
					// Note: ACW rotation is positive, CW rotation is negative.
					float signedAngleDegs = Vector3::getSignedAngleBetweenDegs(hingeReferenceAxis, thisBoneInnerToOuterUV, hingeRotationAxis);

					// Make our bone inner-to-outer UV the hinge reference axis rotated by its maximum clockwise or anticlockwise rotation as required
					if (signedAngleDegs > acwConstraintDegs)
					{
						thisBoneInnerToOuterUV = Vector3::rotateAboutAxisDegrees(hingeReferenceAxis, acwConstraintDegs, hingeRotationAxis).getNormalizedVector();
					}
					else if (signedAngleDegs < cwConstraintDegs)
					{
						thisBoneInnerToOuterUV = Vector3::rotateAboutAxisDegrees(hingeReferenceAxis, cwConstraintDegs, hingeRotationAxis).getNormalizedVector();
					}
				}
			}
			//else if (thisBone.joint.type == L_HINGE)
			//{
			//	// Transform the hinge rotation axis to be relative to the previous bone in the chain
			//	Vector3 hingeRotationAxis = thisBone.joint.rotAxis;
			//
			//	// Construct a rotation matrix based on the previous bone's direction
			//	Matrix4 m = createRotationMatrix(prevBoneInnerToOuterUV);
			//
			//	// Transform the hinge rotation axis into the previous bone's frame of reference
			//	Vector3 relativeHingeRotationAxis = m.TransformDirection(hingeRotationAxis);
			//	//Vector3 relativeHingeRotationAxis = crossProduct(m_fakeChain[i + 1].getDirectionUV(), getDirection(m_fakeChain[i + 1].startPos, m_fakeChain[i].endPos));
			//
			//
			//	// Project this bone direction onto the plane described by the hinge rotation axis
			//	// Note: The returned vector is normalised.
			//	thisBoneInnerToOuterUV = thisBoneInnerToOuterUV.projectOntoPlane(relativeHingeRotationAxis);
			//
			//	// Constrain rotation about reference axis if required
			//	float cwConstraintDegs = -thisBone.joint.hingeClockwiseConDegrees;
			//	float acwConstraintDegs = thisBone.joint.hingeCounterClockwiseConDegrees;
			//	if (!(areMostlyEqual(cwConstraintDegs, -max_con_angle_degrees, 0.001f)) && !(areMostlyEqual(acwConstraintDegs, max_con_angle_degrees, 0.001f)))
			//	{
			//		// Calc. the reference axis in local space
			//		Vector3 relativeHingeReferenceAxis = /*m.TransformDirection(thisBone.joint.refernceAxisUVDir);*/ m_fakeChain[i + 1].getDirectionUV();
			//
			//		// Get the signed angle (about the hinge rotation axis) between the hinge reference axis and the hinge-rotation aligned bone UV
			//		// Note: ACW rotation is positive, CW rotation is negative.
			//		float signedAngleDegs = Vector3::getSignedAngleBetweenDegs(relativeHingeReferenceAxis, thisBoneInnerToOuterUV, relativeHingeRotationAxis);
			//
			//		// Make our bone inner-to-outer UV the hinge reference axis rotated by its maximum clockwise or anticlockwise rotation as required
			//		if (signedAngleDegs > acwConstraintDegs)
			//		{
			//			thisBoneInnerToOuterUV = Vector3::rotateAboutAxisDegrees(relativeHingeReferenceAxis, acwConstraintDegs, relativeHingeRotationAxis).getNormalizedVector();
			//		}
			//		else if (signedAngleDegs < cwConstraintDegs)
			//		{
			//			thisBoneInnerToOuterUV = Vector3::rotateAboutAxisDegrees(relativeHingeReferenceAxis, cwConstraintDegs, relativeHingeRotationAxis).getNormalizedVector();
			//		}
			//	}
			//}


			Vector3 newEndLocation = thisBone.startPos + thisBoneInnerToOuterUV*thisBone.length;
			thisBone.endPos = newEndLocation;

			if (i > 0)
			{
				m_fakeChain[i - 1].startPos = newEndLocation;
			}

			//ASSERT_RECOVERABLE(calcDistanceSquared(thisBone.startPos, thisBone.endPos) == thisBone.length*thisBone.length, "Error: bone did not maintain length");
		}
		else //Changing the root!
		{
			//Root is always a fixed position so set it to the chain's root pos.
			thisBone.startPos = chain->getRootPosition_Global();

			
			//{//Unconstrained
			//	Vector3 newLocation = thisBone.startPos + thisBone.getDirectionUV() * thisBone.length;
			//	thisBone.endPos = newLocation;
			//	if(i > 0)
			//		m_fakeChain[i - 1].startPos = newLocation;
			//}
			/*/
			{//Global Rotor
				// Get the inner-to-outer direction of this bone
				Vector3 thisBoneInnerToOurterUV = thisBone.getDirectionUV();

				float angleBetweenDegrees = Vector3::getAngleBetweenDegrees(BaseboneConstraintUV, thisBoneInnerToOurterUV);
				float constraintAngleDegrees = thisBone.joint.rotorConDegrees;
				if (angleBetweenDegrees > constraintAngleDegrees)
				{
					thisBoneInnerToOurterUV = Vector3::getAngleLimitedUnitVectorDegrees(thisBoneInnerToOurterUV, BaseboneRelativeConstaintUV, constraintAngleDegrees);
				}

				Vector3 newLoctation = thisBone.startPos + thisBoneInnerToOurterUV * thisBone.length;
				thisBone.endPos = newLoctation;

				if (m_fakeChain.size() > 1)
				{
					m_fakeChain[i - 1].startPos = newLoctation;
				}
			}
			/*/
			{//our root node is a local ball joint
				Vector3 thisBoneInnerToOurterUV = thisBone.getDirectionUV();
				
				if (thisBone.joint.type != NO_FCON)
				{
					float angleBetweenDegrees = Vector3::getAngleBetweenDegrees(BaseboneRelativeConstaintUV, thisBoneInnerToOurterUV);
					float constraintAngleDegrees = thisBone.joint.rotorConDegrees;
					if (angleBetweenDegrees > constraintAngleDegrees)
					{
						thisBoneInnerToOurterUV = Vector3::getAngleLimitedUnitVectorDegrees(thisBoneInnerToOurterUV, BaseboneRelativeConstaintUV, constraintAngleDegrees);
					}
				}
			
				Vector3 newLoctation = thisBone.startPos + thisBoneInnerToOurterUV * thisBone.length;
				thisBone.endPos = newLoctation;
			
				if (m_fakeChain.size() > 1)
				{
					m_fakeChain[i - 1].startPos = newLoctation;
				}
			}
			//*/

			//ASSERT_RECOVERABLE(calcDistance(thisBone.startPos, thisBone.endPos) == thisBone.length, C_STRF("Error: bone did not maintain length. calc = %f, real = %f", calcDistance(thisBone.startPos, thisBone.endPos), thisBone.length));
		}
	}
	//*/

	//PROFILE_SCOPE_FUNCTION();
	////Because we are never actually moving the root, we don't need to reset it to the origin position.
	//for (uint i = chain->getNumberOfLinks() - 1; i > 0; --i)
	//{
	//	Vector3 position = m_globalPositions[i];
	//	Vector3 coneVector = getDirection(position, m_globalPositions[i - 1]);
	//	float dist = calcDistance(position, m_globalPositions[i - 1]);
	//	float ratio = chain->getLinksLength(i - 1) / dist;
	//
	//	Vector3 nextJointsNewPosition = (1 - ratio)*position + ratio*m_globalPositions[i - 1]; //p_i+1 = (1 - l_i)p_i + l_i * t
	//	IKConstraint* currentConstraint = chain->getLinksJoint(i)->m_constraint;
	//	if (currentConstraint == nullptr)
	//	{
	//		m_globalPositions[i - 1] = (nextJointsNewPosition);
	//		continue;
	//	}
	//	else
	//	{
	//		Matrix4 lookAtPosMat;// = Matrix4::CreateLookAtMatrix(position, position + coneVector);
	//		lookAtPosMat.setIBasis(position + coneVector); //Setting the forward vector?
	//		lookAtPosMat.setTranslation(position);
	//		Vector3 posTranslation = Constrain(nextJointsNewPosition - position, coneVector, lookAtPosMat, currentConstraint);
	//		m_globalPositions[i - 1] = position + posTranslation;
	//	}
	//}
}

const Vector3 FABRIKConstrainedSolver::Constrain(const Vector3& curToNewPosDisp, const Vector3& direction, const Matrix4& mat, IKConstraint* /*constraint*/) const
{
	//Using ROBLOX FABRIK Page for information on how to constrain 
	float scalar = dotProduct(curToNewPosDisp.getNormalizedVector(), direction);
	Vector3 proj = scalar * direction;


	//get axis that are closest
	Vector3 ups[] = { mat.TransformDirection(Vector3::YAXIS), mat.TransformDirection(-Vector3::YAXIS) };
	Vector3 rights[] = { mat.TransformDirection(Vector3::ZAXIS),  mat.TransformDirection(-Vector3::ZAXIS) };

	Vector3 upvec = ups[0];
	Vector3 rightvec = rights[0];
	if (calcDistanceSquared(ups[0], curToNewPosDisp) > calcDistanceSquared(ups[1], curToNewPosDisp))
		upvec = ups[1];
	if (calcDistanceSquared(rights[0], curToNewPosDisp) > calcDistanceSquared(rights[1], curToNewPosDisp))
		rightvec = rights[1];

	//get the vector from the projection to the calculated vector
	Vector3 adjust = curToNewPosDisp - proj;
	if (scalar < 0)
	{
		proj = -proj;
	}

	//get the 2D components
	float xaspect = dotProduct(adjust, rightvec);
	float yaspect = dotProduct(adjust, upvec);

	//get the cross section of the cone
	float left	=  -(proj.calcLength() * tan(/*constraint->m_minAngles.y*/ .0f * EV1::DEG2RAD));
	float right =  (proj.calcLength() * tan(/*constraint->m_maxAngles.y*/ .0f * EV1::DEG2RAD));
	float up	=  (proj.calcLength() * tan(/*constraint->m_maxAngles.z*/ .0f * EV1::DEG2RAD));
	float down	=  -(proj.calcLength() * tan(/*constraint->m_minAngles.z*/ .0f * EV1::DEG2RAD));

	//find the quadrant
	float xbound = xaspect >= 0 ? right : left;
	float ybound = yaspect >= 0 ? up : down;

	Vector3 f = curToNewPosDisp;
	//check if in 2D point lies in the ellipse
	float ellipse = (xaspect *xaspect) / (xbound * xbound) + (yaspect * yaspect) / (ybound * ybound);
	bool inbounds = ellipse <= 1.f && scalar >= 0.f;

	if (!inbounds)
	{
		// get the angle of our out of ellipse point
		float a = atan2(yaspect, xaspect);
		//find nearest point
		float x = xbound * cos(a);
		float y = ybound * sin(a);
		//convert back to 3D
		Vector3 tempDir = (proj + (rightvec * x) + (upvec * y)).getNormalizedVector();
		f = tempDir * curToNewPosDisp.calcLength();
		return f;
	}
	else
	{
		//DebuggerPrintf("Was inbounds!");
	}
	// return our final vector
	return f;
}

Vector3 FabrikBoneHelper::getDirectionUV()
{
	return getDirection(startPos, endPos);
}
