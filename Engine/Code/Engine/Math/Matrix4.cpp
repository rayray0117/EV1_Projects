#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vcruntime_string.h>
#include <math.h>
#include <utility>
#include "../Core/Profiler.hpp"
#include "../Core/Command.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "../Core/ParseUtils.hpp"
#include "../Core/ErrorWarningAssert.hpp"
#include "../Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
Matrix4::Matrix4()
{
	makeIdentity();
}

Matrix4::Matrix4(const Matrix4& copy)
{
	for(int i = 0; i<16; i++)
		values[i] = copy.values[i];
}

Matrix4::Matrix4(const float* arrayOfFloats)
{
	memcpy(values, arrayOfFloats, sizeof(arrayOfFloats));
}

Matrix4::Matrix4(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation /*= Vector2(0.f, 0.f)*/)
{
	makeIdentity();
	values[0] = iBasis.x;
	values[1] = iBasis.y;
	values[4] = jBasis.x;
	values[5] = jBasis.y;
	values[12] = translation.x;
	values[13] = translation.y;
}

Matrix4::Matrix4(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation /*= Vector3(0.f, 0.f, 0.f)*/)
{
	makeIdentity();
	values[0] = iBasis.x;
	values[1] = iBasis.y;
	values[2] = iBasis.z;
	values[4] = jBasis.x;
	values[5] = jBasis.y;
	values[6] = jBasis.z;
	values[8] = kBasis.x;
	values[9] = kBasis.y;
	values[10] = kBasis.z;
	values[12] = translation.x;
	values[13] = translation.y;
	values[14] = translation.z;
}

Matrix4::Matrix4(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation /*= Vector4(0.f, 0.f, 0.f, 1.f)*/)
{
	values[0] = iBasis.x;
	values[1] = iBasis.y;
	values[2] = iBasis.z;
	values[3] = iBasis.w;
	values[4] = jBasis.x;
	values[5] = jBasis.y;
	values[6] = jBasis.z;
	values[7] = jBasis.w;
	values[8] = kBasis.x;
	values[9] = kBasis.y;
	values[10] = kBasis.z;
	values[11] = kBasis.w;
	values[12] = translation.x;
	values[13] = translation.y;
	values[14] = translation.z;
	values[15] = translation.w;
}

Matrix4::~Matrix4()
{
}

float* Matrix4::GetAsFloatArray()
{
	return values;
}

const float* Matrix4::GetAsFloatArray() const
{
	return values;
}

void Matrix4::makeIdentity()
{
	memset(values, 0, sizeof(values));
	values[0] = 1.f;
	values[5] = 1.f;
	values[10] = 1.f;
	values[15] = 1.f;
}

void Matrix4::concatenateTransform(Matrix4 other)
{
	//PROFILE_SCOPE_FUNCTION();
	Matrix4 temp = *this * other;
	for (int i = 0; i < 16; i++)
		values[i] = temp.values[i];
}

const Matrix4 Matrix4::getTransformed(Matrix4 other) const
{
	Matrix4 result;
	// Ibasis
	result.values[0] = (values[0]  * other.values[0]) + (values[1]  * other.values[4]) + (values[2] * other.values[8])  + (values[3] * other.values[12]);
	result.values[1] = (values[0]  * other.values[1]) + (values[1]  * other.values[5]) + (values[2] * other.values[9])  + (values[3] * other.values[13]);
	result.values[2] = (values[0]  * other.values[2]) + (values[1]  * other.values[6]) + (values[2] * other.values[10]) + (values[3] * other.values[14]);
	result.values[3] = (values[0]  * other.values[3]) + (values[1]  * other.values[7]) + (values[2] * other.values[11]) + (values[3] * other.values[15]);
	// jbasis
	result.values[4] = (values[4]  * other.values[0]) + (values[5]  * other.values[4]) + (values[6] * other.values[8])  + (values[7] * other.values[12]);
	result.values[5] = (values[4]  * other.values[1]) + (values[5]  * other.values[5]) + (values[6] * other.values[9])  + (values[7] * other.values[13]);
	result.values[6] = (values[4]  * other.values[2]) + (values[5]  * other.values[6]) + (values[6] * other.values[10]) + (values[7] * other.values[14]);
	result.values[7] = (values[4]  * other.values[3]) + (values[5]  * other.values[7]) + (values[6] * other.values[11]) + (values[7] * other.values[15]);
	// kbasis
	result.values[8] =  (values[8]  * other.values[0]) + (values[9]  * other.values[4]) + (values[10] * other.values[8])  + (values[11] * other.values[12]);
	result.values[9] =  (values[8]  * other.values[1]) + (values[9]  * other.values[5]) + (values[10] * other.values[9])  + (values[11] * other.values[13]);
	result.values[10] = (values[8]  * other.values[2]) + (values[9]  * other.values[6]) + (values[10] * other.values[10]) + (values[11] * other.values[14]);
	result.values[11] = (values[8]  * other.values[3]) + (values[9]  * other.values[7]) + (values[10] * other.values[11]) + (values[11] * other.values[15]);
	// wbasis
	result.values[12] = (values[12]  * other.values[0]) + (values[13]  * other.values[4]) + (values[14] * other.values[8])  + (values[15] * other.values[12]);
	result.values[13] = (values[12]  * other.values[1]) + (values[13]  * other.values[5]) + (values[14] * other.values[9])  + (values[15] * other.values[13]);
	result.values[14] = (values[12]  * other.values[2]) + (values[13]  * other.values[6]) + (values[14] * other.values[10]) + (values[15] * other.values[14]);
	result.values[15] = (values[12]  * other.values[3]) + (values[13]  * other.values[7]) + (values[14] * other.values[11]) + (values[15] * other.values[15]);

	return result;
}

Vector2 Matrix4::TransformPosition(const Vector2& position2D) const
{
	Vector2 result;
    Vector4 temp(position2D.x, position2D.y, 0.f, 0.f);
    temp = TransformVector(temp);
    result.setXY(temp.x, temp.y);
	return result;
}

Vector3 Matrix4::TransformPosition(const Vector3& position3D) const
{
	Vector3 result;
    Vector4 temp(position3D.x, position3D.y, position3D.z, 1.f);
    temp = TransformVector(temp);
    result.setXYZ(temp.x, temp.y, temp.z);
	return result;
}

Vector2 Matrix4::TransformDirection(const Vector2& direction2D) const
{
	Vector2 result;
    Vector4 temp(direction2D.x, direction2D.y, 0.f, 0.f);
    temp = TransformVector(temp);
    result.setXY(temp.x, temp.y);
	return result;
}

Vector3 Matrix4::TransformDirection(const Vector3& direction3D) const
{
	Vector3 result;
    Vector4 temp(direction3D.x, direction3D.y, direction3D.z, 0.f);
    temp = TransformVector(temp);
    result.setXYZ(temp.x, temp.y, temp.z);
	return result;
}

Vector4 Matrix4::TransformVector(const Vector4& homogeneousVector) const
{
	Vector4 result;
	result.x = values[0] * homogeneousVector.x + values[4] * homogeneousVector.y + values[8] *  homogeneousVector.z + values[12] * homogeneousVector.w;
	result.y = values[1] * homogeneousVector.x + values[5] * homogeneousVector.y + values[9] *  homogeneousVector.z + values[13] * homogeneousVector.w;
	result.z = values[2] * homogeneousVector.x + values[6] * homogeneousVector.y + values[10] * homogeneousVector.z + values[14] * homogeneousVector.w;
	result.w = values[3] * homogeneousVector.x + values[7] * homogeneousVector.y + values[11] * homogeneousVector.z + values[15] * homogeneousVector.w;
	return result;
}

void Matrix4::Translate(const Vector2& translation2D)
{
	values[3] += translation2D.x;
	values[7] += translation2D.y;
}

void Matrix4::Translate(const Vector3& translation3D)
{
	values[3] += translation3D.x;
	values[7] += translation3D.y;
	values[11] += translation3D.z;
}

void Matrix4::Scale(float uniformScale)
{
	for (int i = 0; i < 16; i++)
		values[i] *= uniformScale;
}

void Matrix4::Scale(const Vector2& nonUniformScale2D)
{
	concatenateTransform(CreateScale(nonUniformScale2D));
}

void Matrix4::Scale(const Vector3& nonUniformScale3D)
{
	//PROFILE_SCOPE_FUNCTION();
	concatenateTransform(CreateScale(nonUniformScale3D));
}

void Matrix4::RotateDegreesAboutX(float degrees)
{
	concatenateTransform(CreateRotationDegreesAboutX(degrees));
}

void Matrix4::RotateDegreesAboutY(float degrees)
{
	concatenateTransform(CreateRotationDegreesAboutY(degrees));
}

void Matrix4::RotateDegreesAboutZ(float degrees)
{
	concatenateTransform(CreateRotationDegreesAboutZ(degrees));
}

void Matrix4::RotateRadiansAboutX(float radians)
{
	concatenateTransform(CreateRotationRadiansAboutX(radians));
}

void Matrix4::RotateRadiansAboutY(float radians)
{
	concatenateTransform(CreateRotationRadiansAboutY(radians));
}

void Matrix4::RotateRadiansAboutZ(float radians)
{
	concatenateTransform(CreateRotationRadiansAboutZ(radians));
}

//////////////////////////////////////////////////////////////////////////
//Statics
Matrix4 Matrix4::CreateTranslation(const Vector2& translation2D)
{
	Matrix4 result;
	result.values[3] = translation2D.x;
	result.values[7] = translation2D.y;
	return result;
}

Matrix4 Matrix4::CreateTranslation(const Vector3& translation3D)
{
	Matrix4 result;
	result.values[12] = translation3D.x;
	result.values[13] = translation3D.y;
	result.values[14] = translation3D.z;
	return result;
}

Matrix4 Matrix4::CreateScale(float uniformScale)
{
	Matrix4 result;
	result.values[0] = uniformScale;
	result.values[5] = uniformScale;
	result.values[10] = uniformScale;
	return result;
}

Matrix4 Matrix4::CreateScale(const Vector2& nonUniformScale2D)
{
	Matrix4 result;
	result.values[0] = nonUniformScale2D.x;
	result.values[5] = nonUniformScale2D.y;
	return result;
}

Matrix4 Matrix4::CreateScale(const Vector3& nonUniformScale3D)
{
	//PROFILE_SCOPE_FUNCTION();
	Matrix4 result;
	result.values[0] = nonUniformScale3D.x;
	result.values[5] = nonUniformScale3D.y;
	result.values[10] = nonUniformScale3D.z;
	return result;
}

Matrix4 Matrix4::CreateRotationDegreesAboutX(float degrees)
{
	float cosD = cosDegrees(degrees);
	float sinD = sinDegrees(degrees);
	Matrix4 result;
	result.values[5] = cosD;
	result.values[6] = sinD;
	result.values[9] = -sinD;
	result.values[10] = cosD;
	return result;
}

Matrix4 Matrix4::CreateRotationDegreesAboutY(float degrees)
{
	float cosD = cosDegrees(degrees);
	float sinD = sinDegrees(degrees);
	Matrix4 result;
	result.values[0] = cosD;
	result.values[2] = -sinD;
	result.values[8] = sinD;
	result.values[10] = cosD;
	return result;
}

Matrix4 Matrix4::CreateRotationDegreesAboutZ(float degrees)
{
	float cosD = cosDegrees(degrees);
	float sinD = sinDegrees(degrees);
	Matrix4 result;
	result.values[0] = cosD;
	result.values[1] = sinD;
	result.values[4] = -sinD;
	result.values[5] = cosD;
	return result;
}

Matrix4 Matrix4::CreateRotationRadiansAboutX(float radians)
{
	float cosR = cosf(radians);
	float sinR = sinf(radians);
	Matrix4 result;
	result.values[5] = cosR;
	result.values[6] = sinR;
	result.values[9] = -sinR;
	result.values[10] = cosR;
	return result;
}

Matrix4 Matrix4::CreateRotationRadiansAboutY(float radians)
{
	float cosR = cosf(radians);
	float sinR = sinf(radians);
	Matrix4 result;
	result.values[0] = cosR;
	result.values[2] = -sinR;
	result.values[8] = sinR;
	result.values[10] = cosR;
	return result;
}

Matrix4 Matrix4::CreateRotationRadiansAboutZ(float radians)
{
	float cosR = cosf(radians);
	float sinR = sinf(radians);
	Matrix4 result;
	result.values[0] = cosR;
	result.values[1] = sinR;
	result.values[4] = -sinR;
	result.values[5] = cosR;
	return result;
}





Matrix4 Matrix4::CreateRotation(const Vector3& axis, float degrees)
{
	float angle = convertDegreesToRadians(degrees);
	float c = cosf(angle);
	float s = sinf(angle);
	float t = 1.0f - c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(a1.x*a1.x + a1.y*a1.y + a1.z*a1.z);
	// if (magnitude==0) throw error;
	// a1.x /= magnitude;
	// a1.y /= magnitude;
	// a1.z /= magnitude;

	Matrix4 result;
	result.values[0] = c + axis.x*axis.x*t;
	result.values[to1DMatixValues(1,1)] = c + axis.y*axis.y*t;
	result.values[to1DMatixValues(2,2)] = c + axis.z*axis.z*t;


	float tmp1 = axis.x*axis.y*t;
	float tmp2 = axis.z*s;
	result.values[to1DMatixValues(0,1)] = tmp1 + tmp2;
	result.values[to1DMatixValues(1,0)] = tmp1 - tmp2;
	tmp1 = axis.x*axis.z*t;
	tmp2 = axis.y*s;
	result.values[to1DMatixValues(0,2)] = tmp1 - tmp2;
	result.values[to1DMatixValues(2,0)] = tmp1 + tmp2;    tmp1 = axis.y*axis.z*t;
	tmp2 = axis.x*s;
	result.values[to1DMatixValues(1,2)] = tmp1 + tmp2;
	result.values[to1DMatixValues(2,1)] = tmp1 - tmp2;

	return result;
}

Matrix4 Matrix4::CreateRotationFromDirection(const Vector3& direction, const Vector3& /*up*/)
{
	Vector3 xaxis = crossProduct(Vector3::XAXIS, direction);
	//xaxis.normalize();

	Vector3 yaxis = crossProduct(direction, xaxis);
	//yaxis.normalize();

	Vector3 column1, column2, column3;
	column1.x = xaxis.x;
	column1.y = yaxis.x;
	column1.z = direction.x;

	column2.x = xaxis.y;
	column2.y = yaxis.y;
	column2.z = direction.y;

	column3.x = xaxis.z;
	column3.y = yaxis.z;
	column3.z = direction.z;

	Matrix4 result;
	result.setIBasis(column1);
	result.setJBasis(column2);
	result.setKBasis(column3);
	result.values[15] = 0;
	return result;
}

COMMAND(mat_dir_tests, "")
{
	Matrix4 xDir = Matrix4::CreateRotationFromDirection(Vector3::XAXIS);
	Matrix4 yDir = Matrix4::CreateRotationFromDirection(Vector3::YAXIS);
	Matrix4 zDir = Matrix4::CreateRotationFromDirection(Vector3::ZAXIS);

	Vector3 xResult = xDir.TransformDirection(Vector3::XAXIS);
	Vector3 yResult = yDir.TransformDirection(Vector3::XAXIS);
	Vector3 zResult = zDir.TransformDirection(Vector3::XAXIS);

	CONSOLE->addMessage("X dir =" + vectorToString(xResult));
	CONSOLE->addMessage("Y dir =" + vectorToString(yResult));
	CONSOLE->addMessage("Z dir =" + vectorToString(zResult));
}

Matrix4 Matrix4::CreateOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight, float nearZ, float farZ)
{
	/*float nx = bottomLeft.x;
	float ny = bottomLeft.y;
	float nz = nearZ;

	float fx = topRight.x;
	float fy = topRight.y;
	float fz = farZ;

	float sx = 1.0f / (fx - nx);
	float sy = 1.0f / (fy - ny);
	float sz = 1.0f / (fz - nz);

	Matrix4 result;
	result.setIBasis(Vector3(2.0f * sx, 0.0f, 0.0f));
	result.setJBasis(Vector3(0.0f, 2.0f * sy, 0.0f));
	result.setKBasis(Vector3(0.0f, 0.0f, sz));
	result.setTranslation(Vector3(-(fx + nx)*sx, -(fy + ny)*sy, -nz*sz));

	return result;*/

	float left = bottomLeft.x;
	float bottom = bottomLeft.y;
	float nz = nearZ;

	float right = topRight.x;
	float top = topRight.y;
	float fz = farZ;

	float sx = 1.0f / (right - left);
	float sy = 1.0f / (top - bottom);
	float sz = 1.0f / (fz - nz);

	Matrix4 result;
	result.setIBasis(Vector3(2.0f * sx, 0.0f, 0.0f));
	result.setJBasis(Vector3(0.0f, 2.0f * sy, 0.0f));
	result.setKBasis(Vector3(0.0f, 0.0f, sz));
	result.setTranslation(Vector3(-(right + left)*sx, -(top + bottom)*sy, -(fz + nz)*sz));

	return result;
}

Matrix4 Matrix4::CreatePerspectiveProjection(float fovVerticalDegrees, float aspectRatio, float nearZ, float farZ)
{
	/*float size = inverse_f(tanf(convertDegreesToRadians(fovVerticalDegrees) * .5f));

	// scale X or Y depending which dimension is bigger
	float width = size;
	float height = size;
	if (aspectRatio > 1.f)
	{
		width *= inverse_f(aspectRatio);
	}
	else
	{
		height *= aspectRatio;
	}

	float q = inverse_f(farZ - nearZ); //Forseth what does q mean?
	float farZq = farZ * q;


	Matrix4 result;
	result.setIBasis(Vector3(width, 0.f, 0.f));
	result.setJBasis(Vector3(0.f, height, 0.f));
	result.setKBasis(Vector3(0.f, 0.f, farZq));
	result.values[11] = 1.0f;
	result.setTranslation(Vector3(0.f, 0.f, farZq * -nearZ));
	result.values[15] = 0.f;
	return result;*/

	float size = inverse_f(tanf(convertDegreesToRadians(fovVerticalDegrees) * .5f));
	float neg_depth = nearZ - farZ;

	// scale X or Y depending which dimension is bigger
	float width = size;
	float height = size;
	if (aspectRatio > 1.f)
	{
		width *= inverse_f(aspectRatio);
	}
	else
	{
		height *= aspectRatio;
	}

//	float q = inverse_f(farZ - nearZ); //Forseth what does q mean?
//	float farZq = farZ * q;
	float tots = (farZ + nearZ) / neg_depth;


	Matrix4 result;
	result.setIBasis(Vector3(width, 0.f, 0.f));
	result.setJBasis(Vector3(0.f, height, 0.f));
	result.setKBasis(Vector3(0.f, 0.f, tots));
	result.values[11] = -1.0f;
	result.setTranslation(Vector3(0.f, 0.f, 2.f*(nearZ*farZ)/neg_depth));
	result.values[15] = 0.f;
	return result;
}


Matrix4 Matrix4::CreateOtherHandBasisFlip()
{
	Matrix4 otherHand;
	otherHand.values[10] = -1.f;
	return otherHand;
}

#include "MatrixStack.hpp"
Matrix4 Matrix4::CreateFromEuler(const Vector3& euler)
{
	//*/
	Matrix4 eulerMatrix;

	Matrix4 xRot = CreateRotationDegreesAboutX(-euler.x);
	Matrix4 yRot = CreateRotationDegreesAboutY(-euler.y);
	Matrix4 zRot = CreateRotationDegreesAboutZ(-euler.z);

	eulerMatrix = yRot * xRot * zRot;

	return eulerMatrix;
	/*/

	/** this conversion uses NASA standard aeroplane conventions as described on page:
	*   https://www.euclideanspace.com/maths/geometry/rotations/euler/index.htm
	*   Coordinate System: right hand
	*   Positive angle: right hand
	*   Order of euler angles: heading first, then attitude, then bank
	*   matrix row column ordering:
	*   [m00 m01 m02]
	*   [m10 m11 m12]
	*   [m20 m21 m22]*

	Matrix4 eulerMatrix;
	const float& heading = euler.y;
	const float& attitude = euler.z;
	const float& bank = euler.x;
	// Assuming the angles are in radians.
	float ch = cosDegrees(heading);
	float sh = sinDegrees(heading);
	float ca = cosDegrees(attitude);
	float sa = sinDegrees(attitude);
	float cb = cosDegrees(bank);
	float sb = sinDegrees(bank);

	eulerMatrix.values[to1DMatixValues(0,0)] = ch * ca;
	eulerMatrix.values[to1DMatixValues(0,1)] = sh*sb - ch*sa*cb;
	eulerMatrix.values[to1DMatixValues(0,2)] = ch*sa*sb + sh*cb;
	eulerMatrix.values[to1DMatixValues(1,0)] = sa;
	eulerMatrix.values[to1DMatixValues(1,1)] = ca*cb;
	eulerMatrix.values[to1DMatixValues(1,2)] = -ca*sb;
	eulerMatrix.values[to1DMatixValues(2,0)] = -sh*ca;
	eulerMatrix.values[to1DMatixValues(2,1)] = sh*sa*cb + ch*sb;
	eulerMatrix.values[to1DMatixValues(2,2)] = -sh*sa*sb + ch*cb;

	return eulerMatrix;
	//*/
}
//////////////////////////////////////////////////////////////////////////
void Matrix4::operator=(const Matrix4& rightHandSide)
{
	for (int i = 0; i < 16; i++)
		values[i] = rightHandSide.values[i];
}


const Matrix4 Matrix4::operator*(const Matrix4& rightHandSide) const
{
	return getTransformed(rightHandSide);
}

void Matrix4::tranpose()
{
	std::swap(values[1],  values[4]);
	std::swap(values[2],  values[8]);
	std::swap(values[3],  values[12]);
	std::swap(values[6],  values[9]);
	std::swap(values[7],  values[13]);
	std::swap(values[11], values[14]);
}

const Matrix4 Matrix4::getTranpose() const
{
	Matrix4 result(*this);
	result.tranpose();
	return result;
}

// Possibly bad things
void Matrix4::setIBasis(const Vector3& iBasis)
{
	values[0] = iBasis.x;
	values[1] = iBasis.y;
	values[2] = iBasis.z;
}

const Vector3 Matrix4::getIBasis() const
{
	return Vector3(values[0], values[1], values[2]);
}

void Matrix4::setJBasis(const Vector3& jBasis)
{
	values[4] = jBasis.x;
	values[5] = jBasis.y;
	values[6] = jBasis.z;
}

const Vector3 Matrix4::getJBasis() const
{
	return Vector3(values[4], values[5], values[6]);
}

void Matrix4::setKBasis(const Vector3& kBasis)
{
	values[8] = kBasis.x;
	values[9] = kBasis.y;
	values[10] = kBasis.z;
}

const Vector3 Matrix4::getKBasis() const
{
	return Vector3(values[8], values[9], values[10]);
}

void Matrix4::setTranslation(const Vector3& translation)
{
	//PROFILE_SCOPE_FUNCTION();
	values[12] = translation.x;
	values[13] = translation.y;
	values[14] = translation.z;
}

const Matrix4 Matrix4::getInverse() const
{
	const float det = getDeterminant();
	if (det == 0.f)
	{
		return Matrix4();
	}
	
	//UE4 code just straight up
	typedef float Float4x4[4][4];
	const Float4x4& M = *((const Float4x4*)GetAsFloatArray());
	Float4x4 Result;
	float Det[4];
	Float4x4 Tmp;

	Tmp[0][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
	Tmp[0][1] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
	Tmp[0][2] = M[1][2] * M[2][3] - M[1][3] * M[2][2];

	Tmp[1][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
	Tmp[1][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
	Tmp[1][2] = M[0][2] * M[2][3] - M[0][3] * M[2][2];

	Tmp[2][0] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
	Tmp[2][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
	Tmp[2][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

	Tmp[3][0] = M[1][2] * M[2][3] - M[1][3] * M[2][2];
	Tmp[3][1] = M[0][2] * M[2][3] - M[0][3] * M[2][2];
	Tmp[3][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

	Det[0] = M[1][1] * Tmp[0][0] - M[2][1] * Tmp[0][1] + M[3][1] * Tmp[0][2];
	Det[1] = M[0][1] * Tmp[1][0] - M[2][1] * Tmp[1][1] + M[3][1] * Tmp[1][2];
	Det[2] = M[0][1] * Tmp[2][0] - M[1][1] * Tmp[2][1] + M[3][1] * Tmp[2][2];
	Det[3] = M[0][1] * Tmp[3][0] - M[1][1] * Tmp[3][1] + M[2][1] * Tmp[3][2];

	float Determinant = M[0][0] * Det[0] - M[1][0] * Det[1] + M[2][0] * Det[2] - M[3][0] * Det[3];
	const float	RDet = 1.0f / Determinant;

	Result[0][0] = RDet * Det[0];
	Result[0][1] = -RDet * Det[1];
	Result[0][2] = RDet * Det[2];
	Result[0][3] = -RDet * Det[3];
	Result[1][0] = -RDet * (M[1][0] * Tmp[0][0] - M[2][0] * Tmp[0][1] + M[3][0] * Tmp[0][2]);
	Result[1][1] = RDet * (M[0][0] * Tmp[1][0] - M[2][0] * Tmp[1][1] + M[3][0] * Tmp[1][2]);
	Result[1][2] = -RDet * (M[0][0] * Tmp[2][0] - M[1][0] * Tmp[2][1] + M[3][0] * Tmp[2][2]);
	Result[1][3] = RDet * (M[0][0] * Tmp[3][0] - M[1][0] * Tmp[3][1] + M[2][0] * Tmp[3][2]);
	Result[2][0] = RDet * (
		M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
		M[2][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) +
		M[3][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1])
		);
	Result[2][1] = -RDet * (
		M[0][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
		M[2][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
		M[3][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1])
		);
	Result[2][2] = RDet * (
		M[0][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) -
		M[1][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
		M[3][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
		);
	Result[2][3] = -RDet * (
		M[0][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) -
		M[1][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1]) +
		M[2][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
		);
	Result[3][0] = -RDet * (
		M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
		M[2][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) +
		M[3][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
		);
	Result[3][1] = RDet * (
		M[0][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
		M[2][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
		M[3][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1])
		);
	Result[3][2] = -RDet * (
		M[0][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) -
		M[1][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
		M[3][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
		);
	Result[3][3] = RDet * (
		M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
		M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
		M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
		);

	Matrix4 result;
	memcpy(&result, &Result, 16 * sizeof(float));

	return result;
}

const Vector3 Matrix4::getScale() const
{
	Vector3 result;
	result.x = getIBasis().calcLength();
	result.y = getJBasis().calcLength();
	result.z = getKBasis().calcLength();
	return result;
}

const Vector3 Matrix4::getAsEuler() const
{
	Vector3 euler;
	float& heading = euler.y;
	float& pitch = euler.x;
	float& bank = euler.z;

	float h, p, b; //heading, pitch, and bank in radians

	float sp = -values[to1DMatixValues(1, 2)];
	if (sp <= -1.f)
	{
		p = -M_PI_2;
	}
	else if( sp >= 1.0 )
	{
		p = M_PI_2;
	}
	else
	{
		p = asinf(sp);
	}

	//Check for Gimbal lock
	if (sp > .9999f)
	{
		//Looking straight up or down

		b = 0.f;
		h = atan2f(-values[to1DMatixValues(2, 0)], values[to1DMatixValues(0, 0)]);
	}
	else
	{
		h = atan2f(values[to1DMatixValues(0, 2)], values[to1DMatixValues(2, 2)]);
		b = atan2f(values[to1DMatixValues(1, 0)], values[to1DMatixValues(1, 1)]);
	}

	pitch = EV1::RAD2DEG * p;
	heading = EV1::RAD2DEG * h;
	bank = EV1::RAD2DEG * b;

	/*/
	// this conversion uses conventions as described on page:
	//   https://www.euclideanspace.com/maths/geometry/rotations/euler/index.htm
	//   Coordinate System: right hand
	//   Positive angle: right hand
	//   Order of euler angles: heading first, then attitude, then bank
	//   matrix row column ordering:
	//   [m00 m01 m02]
	//   [m10 m11 m12]
	//   [m20 m21 m22]

	Vector3 euler;
	float& heading = euler.y;
	float& attitude = euler.z;
	float& bank = euler.x;

	// Assuming the angles are in radians.
	if (values[to1DMatixValues(1,0)] > 0.998) 
	{ // singularity at north pole
		heading = atan2Degrees(values[to1DMatixValues(0,2)], values[to1DMatixValues(2,2)]);
		attitude = M_PI_2;
		bank = 0;
		return euler;
	}
	if (values[to1DMatixValues(1,0)] < -0.998) 
	{ // singularity at south pole
		heading = atan2Degrees(values[to1DMatixValues(0,2)], values[to1DMatixValues(2,2)]);
		attitude = -M_PI_2;
		bank = 0;
		return euler;
	}
	heading = atan2Degrees(-values[to1DMatixValues(2,0)], values[to1DMatixValues(0,0)]);
	bank = atan2Degrees(-values[to1DMatixValues(1,2)], values[to1DMatixValues(1,1)]);
	attitude = EV1::RAD2DEG * asinf(values[to1DMatixValues(1,0)]);

	//*/
	return euler;
}

//Local Test Function
void EulerMatCheck(const Vector3& euler, uint attempt = 0)
{
	Matrix4 originalMat = Matrix4::CreateFromEuler(euler);
	
	Matrix4 qtest = originalMat;
	Vector3 qeuler = qtest.getAsEuler();
	Matrix4 qMat = Matrix4::CreateFromEuler(qeuler);

	float dotI = dotProduct(originalMat.getIBasis(), qMat.getIBasis());
	ASSERT_OR_DIE(dotI >= .99f, C_STRF("Euler: %s, did not match at IBasis. Attempts = %i", vectorToString(euler).c_str(), attempt));

	float dotJ = dotProduct(originalMat.getJBasis(), qMat.getJBasis());
	ASSERT_OR_DIE(dotJ >= .99f, C_STRF("Euler: %s, did not match at JBasis. Attempts = %i", vectorToString(euler).c_str(), attempt));
	
	float dotK = dotProduct(originalMat.getKBasis(), qMat.getKBasis());
	ASSERT_OR_DIE(dotK >= .99f, C_STRF("Euler: %s, did not match at KBasis. Attempts = %i", vectorToString(euler).c_str(), attempt));
}

Vector3 getRandomEuler2()
{
	Vector3 euler;
	float& x = euler.x;
	float& y = euler.y;
	float& z = euler.z;

	x = truncToFloat(getRandomFloatInRange(-360.f, 360.f));
	y = truncToFloat(getRandomFloatInRange(-360.f, 360.f));
	z = truncToFloat(getRandomFloatInRange(-360.f, 360.f));

	return euler;
}

COMMAND(mat4_euler_test_auto, "Automatic test for converting Eulers to Quats")
{
	Vector3 original;

	for (uint i = 0; i < 3000; i++)
	{
		original = getRandomEuler2();
		EulerMatCheck(original, i);
	}
}


COMMAND(EULER_MATRIX_TEST, "Type in the angles")
{
	std::vector<std::string> args;
	splitArgs(arguments, args);
	Vector3 euler(stof(args[0]), stof(args[1]), stof(args[2]));

	Matrix4 eulerToMat = Matrix4::CreateFromEuler(euler);

	Vector3 matToEuler = eulerToMat.getAsEuler();
	CONSOLE->addMessage(vectorToString(matToEuler));
}