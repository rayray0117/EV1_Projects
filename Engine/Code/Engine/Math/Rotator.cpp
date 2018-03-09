#include "Engine/Math/Rotator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Quaternion.hpp"
#include <math.h>
//////////////////////////////////////////////////////////////////////////
Rotator::Rotator(const Quaternion& quat)
{
	*this = quat.getRotator();
}
//////////////////////////////////////////////////////////////////////////
Vector3 Rotator::getDirection() const
{
	float CP, SP, CY, SY;
	UE4::SinCos(SP, CP, convertDegreesToRadians(pitch));
	UE4::SinCos(SY, CY, convertDegreesToRadians(yaw));
	Vector3 V = Vector3(CP*CY, CP*SY, SP);

	return V;
}

Vector3 Rotator::getAsVector() const
{
	return Vector3(roll, pitch, yaw);
}
/*/
Quaternion Rotator::getAsQuat() const
{
	const float DIVIDE_BY_2 = EV1::DEG2RAD / 2.f;
	float SP, SY, SR;
	float CP, CY, CR;

	UE4::SinCos(SP, CP, pitch*DIVIDE_BY_2);
	UE4::SinCos(SY, CY, yaw*DIVIDE_BY_2);
	UE4::SinCos(SR, CR, roll*DIVIDE_BY_2);

	Quaternion RotationQuat;
	RotationQuat.x = CR*SP*SY - SR*CP*CY;
	RotationQuat.y = -CR*SP*CY - SR*CP*SY;
	RotationQuat.z = CR*CP*SY - SR*SP*CY;
	RotationQuat.w = CR*CP*CY + SR*SP*SY;

	return RotationQuat;
}
//*/
Rotator Rotator::getClamped() const
{
	return Rotator(ClampAxis(pitch), ClampAxis(yaw), ClampAxis(roll));
}

Rotator Rotator::getNormalized() const
{
	Rotator Rot = *this;
	Rot.Normalize();
	return Rot;
}

Rotator Rotator::getDenormalized() const
{
	Rotator Rot = *this;
	Rot.pitch = ClampAxis(Rot.pitch);
	Rot.yaw = ClampAxis(Rot.yaw);
	Rot.roll = ClampAxis(Rot.roll);
	return Rot;
}
//////////////////////////////////////////////////////////////////////////
void Rotator::Normalize()
{
	pitch = NormalizeAxis(pitch);
	yaw = NormalizeAxis(yaw);
	roll = NormalizeAxis(roll);
}

Vector3 Rotator::AxisAngleToEuler(const Vector3& axis, float angle)
{
	Vector3 euler;
	float& heading = euler.y;
	float& attitude = euler.z;
	float& bank = euler.x;

	const float& x = axis.x;
	const float& y = axis.y;
	const float& z = axis.z;

	float s = sinf(angle);
	float c = cosf(angle);
	float t = 1 - c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(x*x + y*y + z*z);
	// if (magnitude==0) throw error;
	// x /= magnitude;
	// y /= magnitude;
	// z /= magnitude;
	if ((x*y*t + z*s) > 0.998f) { // north pole singularity detected
		heading = EV1::RAD2DEG * 2 * atan2f(x*sinf(angle / 2),cosf(angle / 2));
		attitude = EV1::RAD2DEG * M_PI_2;
		bank = 0;
		return euler;
	}
	if ((x*y*t + z*s) < -0.998f) { // south pole singularity detected
		heading = EV1::RAD2DEG * -2 * atan2f(x*sinf(angle / 2), cosf(angle / 2));
		attitude = EV1::RAD2DEG * -M_PI_2;
		bank = 0;
		return euler;
	}
	heading = EV1::RAD2DEG * atan2f(y * s - x * z * t, 1 - (y*y + z*z) * t);
	attitude = EV1::RAD2DEG * asinf(x * y * t + z * s);
	bank = EV1::RAD2DEG * atan2f(x * s - y * z * t, 1 - (x*x + z*z) * t);
	
	return euler;
}
/*/
void Rotator::EulerToAxisAngle(const Vector3& euler, Vector3& axis, float& angleDegrees)
{
	const float& heading = euler.y;
	const float& attitude = euler.z;
	const float& bank = euler.x;

	// Assuming the angles are in radians.
	double c1 = cosDegrees(heading / 2);
	double s1 = sinDegrees(heading / 2);
	double c2 = cosDegrees(attitude / 2);
	double s2 = sinDegrees(attitude / 2);
	double c3 = cosDegrees(bank / 2);
	double s3 = sinDegrees(bank / 2);
	double c1c2 = c1*c2;
	double s1s2 = s1*s2;
	w = c1c2*c3 - s1s2*s3;
	x = c1c2*s3 + s1s2*c3;
	y = s1*c2*c3 + c1*s2*s3;
	z = c1*s2*c3 - s1*c2*s3;
	angle = 2 * Math.acos(w);
	double norm = x*x + y*y + z*z;
	if (norm < 0.001) { // when all euler angles are zero angle =0 so
						// we can set axis to anything to avoid divide by zero
		x = 1;
		y = z = 0;
	}
	else {
		norm = Math.sqrt(norm);
		x /= norm;
		y /= norm;
		z /= norm;
	}
}
//*/
////////////////////////////////////////////////////////////////////////// static functions //////////////////////////////////////////////////////////////////////////
float Rotator::ClampAxis(float Angle)
{
	// returns Angle in the range (-360,360)
	Angle = UE4::FloatMod(Angle, 360.f);

	if (Angle < 0.f)
	{
		// shift to [0,360) range
		Angle += 360.f;
	}

	return Angle;
}

float Rotator::NormalizeAxis(float Angle)
{
	// returns Angle in the range [0,360)
	Angle = ClampAxis(Angle);

	if (Angle > 180.f)
	{
		// shift to (-180,180]
		Angle -= 360.f;
	}

	return Angle;
}

Rotator Rotator::MakeFromEuler(const Vector3& euler)
{
	return Rotator(euler.y, -euler.z, euler.x);
}

Rotator Rotator::MakeFromDirection(const Vector3& dir)
{
	Rotator result;
	result.yaw = atan2Degrees(dir.y, dir.x);
	result.pitch = atan2Degrees(dir.z, Vector2(dir).calcLength());
	result.roll = 0.f;
	return result;
}
