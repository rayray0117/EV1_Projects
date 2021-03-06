#pragma once
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Rotator.hpp"
//////////////////////////////////////////////////////////////////////////
// Note: Using UE4 as a reference for Quaternions since they are so weird
//////////////////////////////////////////////////////////////////////////
class Quaternion
{
public:
	Quaternion(const Matrix4& matrix);
	Quaternion(const Vector3& axis, float angleDegrees);
	Quaternion():x(0), y(0), z(0), w(1){}
	~Quaternion() {}
	
	Vector3 RotateVector(const Vector3& vector) const;
	Matrix4 getAsMatrix(); //This will normalize the quaternion!
	const Matrix4 getAsMatrix() const;

	void SetFromEuler(const Vector3& angles);
	void SetFromRotator(const Rotator& angles);
	Vector3 getEulerAngles() const;
	Rotator getRotator() const;

	void Normalize();
	float calcLength() const;
	float calcLengthSquared() const;
	Quaternion getInverse() const { Quaternion q = *this; q.Normalize(); q.x = -q.x, q.y = -q.y, q.z = -q.z; }

	void operator*=(const Quaternion& rightHandSide);
	const Quaternion operator*(const Quaternion& rightHandSide);
	const bool operator!=(const Quaternion& right) const;

	friend const Quaternion interpolate(const Quaternion& start, const Quaternion& end, float fractionToEnd);
	friend const Quaternion slerp_notNormalized(const Quaternion& start, const Quaternion& end, float fractionToEnd);
	//friend const Quaternion slerp(const Quaternion& start, const Quaternion& end, float fractionToEnd);

	Vector3 getRotationAxis() const;
	void getAxisAndAngle(Vector3& axis, float& angle) const;

	static Quaternion CreateRotateObjectToInertial(const Vector3& euler);
	static Quaternion CreateRotateInertialToObject(const Vector3& euler);
	static Quaternion CreateFromAxisAndAngleInRads(const Vector3& euler, float angleRadians);
	static Quaternion SolveAim(const Vector3& currentPosition, const Vector3& targetPosition, const Vector3& aimVector);
	static Quaternion FindBetweenNormals(const Vector3& a, const Vector3& b);//From UE4

public:
	float x;
	float y; 
	float z;
	float w;
};
//////////////////////////////////////////////////////////////////////////
// Inline
inline Quaternion::Quaternion(const Vector3& axis, float angleDegrees)
{
	float angleRad = convertDegreesToRadians(angleDegrees);
	const float halfAngle = .5f * angleRad;
	float s, c;

	UE4::SinCos(s, c, halfAngle);

	x = s * axis.x;
	y = s * axis.y;
	z = s * axis.z;
	w = c;
}

inline Vector3 Quaternion::RotateVector(const Vector3& vector) const
{
	const Vector3 qVector(x, y, z);
	const Vector3 tVector = crossProduct(qVector, vector);
	const Vector3 result = vector + (w * tVector) + crossProduct(qVector, tVector);
	return result;
}