#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
//////////////////////////////////////////////////////////////////////////
// Matrix Memory is set to Row Major Format
// Vector4 = [ x y z w ] and is a [1x4] Matrix
// So multipication is Vector4 * Matrix4
// See https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/row-major-vs-column-major-vector for row vs column explanation
//////////////////////////////////////////////////////////////////////////
class Matrix4
{
public:
	Matrix4();
	Matrix4(const Matrix4& copy);
	Matrix4(const float* arrayOfFloats);
	Matrix4(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2(0.f, 0.f));
	Matrix4(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3(0.f, 0.f, 0.f));
	Matrix4(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation = Vector4(0.f, 0.f, 0.f, 1.f));
	~Matrix4();

	const float* GetAsFloatArray() const;	// Used when the Matrix4 in question in const
	float* 	 GetAsFloatArray();		// Used when the Matrix4 is not const

	void makeIdentity();
	void concatenateTransform(Matrix4 other);
	const Matrix4 getTransformed(Matrix4 other) const; 
	//To get my world "World = My Local * Parent's World"  TODO
	//To get my local "Local = My World * Parent's Local Inverse" this.getTransformed(parent_world.getInverse); this is in global transform


	Vector2 TransformPosition(const Vector2& position2D) const; // Assumes z=0, w=1
	Vector3 TransformPosition(const Vector3& position3D) const; // Assumes w=1
	Vector2 TransformDirection(const Vector2& direction2D) const; // Assumes z=0, w=0
	Vector3 TransformDirection(const Vector3& direction3D) const; // Assumes w=0
	Vector4 TransformVector(const Vector4& homogeneousVector) const; // w is provided

	void Translate(const Vector2& translation2D); // z translation assumed to be 0
	void Translate(const Vector3& translation3D);
	void Scale(float uniformScale);
	void Scale(const Vector2& nonUniformScale2D); // z scale assumed to be 1
	void Scale(const Vector3& nonUniformScale3D);
	void RotateDegreesAboutX(float degrees);
	void RotateDegreesAboutY(float degrees);
	void RotateDegreesAboutZ(float degrees); // Possibly also offered as RotateDegrees2D
	void RotateRadiansAboutX(float radians);
	void RotateRadiansAboutY(float radians);
	void RotateRadiansAboutZ(float radians); // Possibly also offered as RotateRadians2D

	static Matrix4 CreateTranslation(const Vector2& translation2D); // z translation assumed to be 0
	static Matrix4 CreateTranslation(const Vector3& translation3D);
	static Matrix4 CreateScale(float uniformScale);
	static Matrix4 CreateScale(const Vector2& nonUniformScale2D); // z scale assumed to be 1
	static Matrix4 CreateScale(const Vector3& nonUniformScale3D);
	static Matrix4 CreateRotationDegreesAboutX(float degrees);
	static Matrix4 CreateRotationDegreesAboutY(float degrees);
	static Matrix4 CreateRotationDegreesAboutZ(float degrees); // a.k.a. CreateRotationDegrees2D
	static Matrix4 CreateRotationRadiansAboutX(float radians);
	static Matrix4 CreateRotationRadiansAboutY(float radians);
	static Matrix4 CreateRotationRadiansAboutZ(float radians); // a.k.a. CreateRotationRadians2D
	static Matrix4 CreateRotation(const Vector3& axis, float degrees);
	static Matrix4 CreateRotationFromDirection(const Vector3& direction, const Vector3& up = Vector3::YAXIS);

	static Matrix4 CreateOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight, float nearZ, float farZ);
	static Matrix4 CreatePerspectiveProjection(float fovVerticalDegrees,float aspectRatio, float nearZ, float farZ);
	static Matrix4 CreateOtherHandBasisFlip();
	static Matrix4 CreateFromEuler(const Vector3& euler);


	void operator=(const Matrix4& rightHandSide);
	const Matrix4 operator*(const Matrix4& rightHandSide) const;


	void tranpose();
	const Matrix4 getTranpose() const;

	//Probably bad don't use lightly
	void setIBasis(const Vector3& iBasis);
	void setJBasis(const Vector3& jBasis);
	void setKBasis(const Vector3& kBasis);
	void setTranslation(const Vector3& translation);

	const Vector3 getIBasis() const;
	const Vector3 getJBasis() const;
	const Vector3 getKBasis() const;
	const Vector3 getTranslation() const;

	// Useful UE4 functions
	const float getDeterminant() const;
	const float getRotDeterminant() const;
	const Vector3 getScaledAxis(const Vector3& axis) const;
	const Matrix4 getInverse() const;
	const Vector3 getScale() const;

	const Vector3 getAsEuler() const;
public:
	float values[16];
};

//////////////////////////////////////////////////////////////////////////
// Inlines
inline const Vector3 Matrix4::getTranslation() const
{
	return Vector3(values[12], values[13], values[14]);
}

inline const float Matrix4::getDeterminant() const
{
// 	return	M[0][0] * (
// 		M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
// 		M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
// 		M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
// 		) -
// 		M[1][0] * (
// 			M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
// 			M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
// 			M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
// 			) +
// 		M[2][0] * (
// 			M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
// 			M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
// 			M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
// 			) -
// 		M[3][0] * (
// 			M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
// 			M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
// 			M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
// 			);
	return	values[0] * (
		values[5] * (values[10] * values[15] - values[11] * values[14]) -
		values[9] * (values[6] * values[15] - values[7] * values[14]) +
		values[13] * (values[6] * values[11] - values[7] * values[10])
		) -
		values[4] * (
			values[1] * (values[10] * values[15] - values[11] * values[14]) -
			values[9] * (values[2] * values[15] - values[3] * values[14]) +
			values[13] * (values[2] * values[11] - values[3] * values[10])
			) +
		values[8] * (
			values[1] * (values[6] * values[15] - values[7] * values[14]) -
			values[5] * (values[2] * values[15] - values[3] * values[14]) +
			values[13] * (values[2] * values[7] - values[3] * values[6])
			) -
		values[12] * (
			values[1] * (values[6] * values[11] - values[7] * values[10]) -
			values[5] * (values[2] * values[11] - values[3] * values[10]) +
			values[9] * (values[2] * values[7]  - values[3] * values[6])
			);
}

inline const float Matrix4::getRotDeterminant() const
{
// 	return
// 		M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
// 		M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
// 		M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1]);

	return
		values[0] * (values[5] * values[10] - values[6] * values[9]) -
		values[4] * (values[1] * values[10] - values[2] * values[9]) +
		values[8] * (values[1] * values[6] - values[2] * values[5]);
}

inline const Vector3 Matrix4::getScaledAxis(const Vector3& axis) const
{
// 	switch (InAxis)
// 	{
// 	case EAxis::X:
// 		return FVector(M[0][0], M[0][1], M[0][2]);
// 
// 	case EAxis::Y:
// 		return FVector(M[1][0], M[1][1], M[1][2]);
// 
// 	case EAxis::Z:
// 		return FVector(M[2][0], M[2][1], M[2][2]);
// 
// 	default:
// 		ensure(0);
// 		return FVector::ZeroVector;
// 	}
	if (axis == Vector3::XAXIS)
	{
		return Vector3(values[0], values[1], values[2]);
	}
	else if (axis == Vector3::YAXIS)
	{
		return Vector3(values[4], values[5], values[6]);
	}
	else if (axis == Vector3::ZAXIS)
	{
		return Vector3(values[8], values[9], values[10]);
	}
	else
	{
		return Vector3::ZERO;
	}
}