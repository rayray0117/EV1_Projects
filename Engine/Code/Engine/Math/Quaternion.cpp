#include "Engine/Math/Quaternion.hpp"
#include "Engine/Core/EngineBase.hpp"
#include <math.h>
#include "Engine/Core/Command.hpp"
#include <vector>
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/Console.hpp"
#include "../Core/ErrorWarningAssert.hpp"
#include "../Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
float FloatSelect(float Comparand, float ValueGEZero, float ValueLTZero)
{
	return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
}

float qAcos(float Value) { return acosf((Value<-1.f) ? -1.f : ((Value<1.f) ? Value : 1.f)); }
//////////////////////////////////////////////////////////////////////////
Quaternion::Quaternion(const Matrix4& matrix)
{
	/*/ Old UE4 Code
	float s;

	const float trace = matrix.values[0] + matrix.values[5] + matrix.values[10];

	if (trace > 0.f)
	{
		float InvS = inverseSqrt_f(trace + 1.f);
		w = .5f * inverse_f(InvS);
		s = .5f * InvS;

		x = (matrix.values[to1DMatixValues(1,2)] - matrix.values[to1DMatixValues(2,1)]) * s;
		y = (matrix.values[to1DMatixValues(2,0)] - matrix.values[to1DMatixValues(0,2)]) * s;
		z = (matrix.values[to1DMatixValues(0,1)] - matrix.values[to1DMatixValues(1,0)]) * s;
	}
	else
	{
		// diagonal is negative
		int i = 0;

		if (matrix.values[to1DMatixValues(1,1)] > matrix.values[to1DMatixValues(0,0)])
			i = 1;

		if (matrix.values[to1DMatixValues(2,2)] > matrix.values[to1DMatixValues(i, i)])
			i = 2;

		static const int nxt[3] = { 1, 2, 0 };
		const int j = nxt[i];
		const int k = nxt[j];

		s = matrix.values[to1DMatixValues(i, i)] - matrix.values[to1DMatixValues(j,j)] - matrix.values[to1DMatixValues(k,k)] + 1.0f;

		float InvS = inverseSqrt_f(s);

		float qt[4];
		qt[i] = 0.5f * (1.f / InvS);

		s = 0.5f * InvS;

		qt[3] = (matrix.values[to1DMatixValues(j,k)] - matrix.values[to1DMatixValues(k,j)]) * s;
		qt[j] = (matrix.values[to1DMatixValues(i,j)] + matrix.values[to1DMatixValues(j,i)]) * s;
		qt[k] = (matrix.values[to1DMatixValues(i,k)] + matrix.values[to1DMatixValues(k,i)]) * s;

		x = qt[0];
		y = qt[1];
		z = qt[2];
		w = qt[3];
	}
	//*/

	double qw, qx, qy, qz; //Start them off as doubles and then lower them to floats.

	//Code follows Quaternion Math from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	const float trace = matrix.values[0] + matrix.values[5] + matrix.values[10]; //The diagonal
	if (trace > 0.f)
	{
		double s = sqrt(trace + 1.0) * 2; //
		qw = .25 * s;
		double InvS = inverse_d(s);

		qx = (matrix.values[to1DMatixValues(1, 2)] - matrix.values[to1DMatixValues(2, 1)]) * InvS;
		qy = (matrix.values[to1DMatixValues(2, 0)] - matrix.values[to1DMatixValues(0, 2)]) * InvS;
		qz = (matrix.values[to1DMatixValues(0, 1)] - matrix.values[to1DMatixValues(1, 0) ]) * InvS;
	}
	else if ((matrix.values[to1DMatixValues(0, 0)] > matrix.values[to1DMatixValues(1, 1)])&(matrix.values[to1DMatixValues(0, 0)] > matrix.values[to1DMatixValues(2, 2)]))
	{
		double S = sqrt(1.0 + matrix.values[to1DMatixValues(0, 0)] - matrix.values[to1DMatixValues(1, 1)] - matrix.values[to1DMatixValues(2, 2)]) * 2; // S=4*qx 
		double InvS = inverse_d(S);//Save time by doing one divide then multiply

		qx = 0.25 * S;
		qw = (matrix.values[to1DMatixValues(1, 2)] - matrix.values[to1DMatixValues(2, 1)]) * InvS;
		qy = (matrix.values[to1DMatixValues(1, 0)] + matrix.values[to1DMatixValues(0, 1)]) * InvS;
		qz = (matrix.values[to1DMatixValues(2, 0)] + matrix.values[to1DMatixValues(0, 2)]) * InvS;
	}
	else if (matrix.values[to1DMatixValues(1,1)] > matrix.values[to1DMatixValues(2,2)])
	{
		double S = sqrt(1.0 + matrix.values[to1DMatixValues(1,1)] - matrix.values[to1DMatixValues(0,0)] - matrix.values[to1DMatixValues(2,2)]) * 2; // S=4*qy
		double InvS = inverse_d(S);//Save time by doing one divide then multiply

		qw = (matrix.values[to1DMatixValues(2, 0)] - matrix.values[to1DMatixValues(0, 2)]) * InvS;
		qx = (matrix.values[to1DMatixValues(1, 0)] + matrix.values[to1DMatixValues(0, 1)]) * InvS;
		qz = (matrix.values[to1DMatixValues(2, 1)] + matrix.values[to1DMatixValues(1, 2)]) * InvS;
		qy = 0.25 * S;
	}
	else 
	{
		double S = sqrt(1.0 + matrix.values[to1DMatixValues(2,2)] - matrix.values[to1DMatixValues(0,0)] - matrix.values[to1DMatixValues(1,1)]) * 2; // S=4*qz
		double InvS = inverse_d(S);//Save time by doing one divide then multiply

		qw = (matrix.values[to1DMatixValues(0, 1)] - matrix.values[to1DMatixValues(1, 0)]) * InvS;
		qx = (matrix.values[to1DMatixValues(2, 0)] + matrix.values[to1DMatixValues(0, 2)]) * InvS;
		qy = (matrix.values[to1DMatixValues(2, 1)] + matrix.values[to1DMatixValues(1, 2)]) * InvS;
		qz = 0.25 * S;
	}

	w = (float)qw;
	x = (float)qx;
	y = (float)qy;
	z = (float)qz;
}
//////////////////////////////////////////////////////////////////////////
Matrix4 Quaternion::getAsMatrix()
{
	Normalize();
	Matrix4 left;
	left.values[0]  =  w;  left.values[1]  =  z;  left.values[2]  = -y;  left.values[3]  = x;
	left.values[4]  = -z;  left.values[5]  =  w;  left.values[6]  =  x;  left.values[7]  = y;
	left.values[8]  =  y;  left.values[9]  = -x;  left.values[10] =  w;  left.values[11] = z;
	left.values[12] = -x;  left.values[13] = -y;  left.values[14] = -z;  left.values[15] = w;

	Matrix4 right;
	right.values[0]  =  w;  right.values[1]  =  z;  right.values[2]  = -y;  right.values[3]  = -x;
	right.values[4]  = -z;  right.values[5]  =  w;  right.values[6]  =  x;  right.values[7]  = -y;
	right.values[8]  =  y;  right.values[9]  = -x;  right.values[10] =  w;  right.values[11] = -z;
	right.values[12] =  x;  right.values[13] =  y;  right.values[14] =  z;  right.values[15] =  w;

	return left*right;
}
#include "../Core/Profiler.hpp"
const Matrix4 Quaternion::getAsMatrix() const
{
	//PROFILE_SCOPE_FUNCTION();
	Quaternion result = *this;
	result.Normalize();
	Matrix4 left;
	left.values[0] = result.w;  left.values[1] = result.z;  left.values[2] = -result.y;  left.values[3] = result.x;
	left.values[4] = -result.z;  left.values[5] = result.w;  left.values[6] = result.x;  left.values[7] = result.y;
	left.values[8] = result.y;  left.values[9] = -result.x;  left.values[10] = result.w;  left.values[11] = result.z;
	left.values[12] = -result.x;  left.values[13] = -result.y;  left.values[14] = -result.z;  left.values[15] = result.w;

	Matrix4 right;
	right.values[0] = result.w;  right.values[1] = result.z;  right.values[2] = -result.y;  right.values[3] = -result.x;
	right.values[4] = -result.z;  right.values[5] = result.w;  right.values[6] = result.x;  right.values[7] = -result.y;
	right.values[8] = result.y;  right.values[9] = -result.x;  right.values[10] = result.w;  right.values[11] = -result.z;
	right.values[12] = result.x;  right.values[13] = result.y;  right.values[14] = result.z;  right.values[15] = result.w;

	//return right.getTransformed(left);
	return left*right;
}

void Quaternion::SetFromEuler(const Vector3& angles)
{
	/*/ //From https://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
	float heading = angles.y * EV1::DEG2RAD;
	float attitude = angles.z * EV1::DEG2RAD;
	float bank = angles.x * EV1::DEG2RAD;


	float c1 = cosf(heading / 2);
	float s1 = sinf(heading / 2);
	float c2 = cosf(attitude / 2);
	float s2 = sinf(attitude / 2);
	float c3 = cosf(bank / 2);
	float s3 = sinf(bank / 2);
	float c1c2 = c1*c2;
	float s1s2 = s1*s2;
	w = c1c2*c3 - s1s2*s3;
	x = c1c2*s3 + s1s2*c3;
	y = s1*c2*c3 + c1*s2*s3;
	z = c1*s2*c3 - s1*c2*s3;
	//*/

	float heading = angles.y * EV1::DEG2RAD;
	float pitch = angles.x * EV1::DEG2RAD;
	float bank = angles.z * EV1::DEG2RAD;


	float c1 = cosf(heading / 2);
	float s1 = sinf(heading / 2);
	float c2 = cosf(pitch / 2);
	float s2 = sinf(pitch / 2);
	float c3 = cosf(bank / 2);
	float s3 = sinf(bank / 2);
	w =  c1*c2*c3 + s1*s2*s3;
	x = -c1*s2*c3 - s1*c2*s3;
	y =  c1*s2*s3 - s1*c2*c3;
	z =  s1*s2*c3 - c1*c2*s3;
}

//////////////////////////////////////////////////////////////////////////
void Quaternion::SetFromRotator(const Rotator& angles)
{
	//From UE4
	//*/
	const float DIVIDE_BY_2 = EV1::DEG2RAD / 2.f;
	float SP, SY, SR;
	float CP, CY, CR;

	UE4::SinCos(SP, CP, angles.pitch*DIVIDE_BY_2); //pitch
	UE4::SinCos(SY, CY, angles.yaw*DIVIDE_BY_2); //yaw
	UE4::SinCos(SR, CR, angles.roll*DIVIDE_BY_2); //roll

	x = CR*SP*SY - SR*CP*CY;
	y = -CR*SP*CY - SR*CP*SY;
	z = CR*CP*SY - SR*SP*CY;
	w = CR*CP*CY + SR*SP*SY;
	/*/
	float heading	= angles.y * EV1::DEG2RAD;
	float attitude	= angles.z * EV1::DEG2RAD;
	float bank		= angles.x * EV1::DEG2RAD;


	float c1 = cosf(heading / 2);
	float s1 = sinf(heading / 2);
	float c2 = cosf(attitude / 2);
	float s2 = sinf(attitude / 2);
	float c3 = cosf(bank / 2);
	float s3 = sinf(bank / 2);
	float c1c2 = c1*c2;
	float s1s2 = s1*s2;
	w = c1c2*c3 - s1s2*s3;
	x = c1c2*s3 + s1s2*c3;
	y = s1*c2*c3 + c1*s2*s3;
	z = c1*s2*c3 - s1*c2*s3;
	//*/
}

Vector3 Quaternion::getEulerAngles() const
{
	Quaternion q1 = *this;
	q1.Normalize();
	Vector3 euler;
	float& heading = euler.y;
	float& pitch = euler.x;
	float& bank = euler.z;
	/*/

	//From http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	float test = q1.x*q1.y + q1.z*q1.w;
	if (test > 0.499) 
	{ // singularity at north pole
		heading = 2 * atan2f(q1.x, q1.w);
		attitude = M_PI / 2;
		bank = 0;
	}
	else if (test < -0.499) 
	{ // singularity at south pole
		heading = -2 * atan2f(q1.x, q1.w);
		attitude = -M_PI / 2;
		bank = 0;
	}
	else
	{
		float sqx = q1.x*q1.x;
		float sqy = q1.y*q1.y;
		float sqz = q1.z*q1.z;
		heading = atan2f(2 * q1.y*q1.w - 2 * q1.x*q1.z, 1 - 2 * sqy - 2 * sqz);
		attitude = asinf(2 * test);
		bank = atan2f(2 * q1.x*q1.w - 2 * q1.y*q1.z, 1 - 2 * sqx - 2 * sqz);
	}
	


	//Set angles to degrees
	heading = convertRadiansToDegrees(heading);
	attitude = convertRadiansToDegrees(attitude);
	bank = convertRadiansToDegrees(bank);

	return rotator;
	//*/

	float h, p, b;

	float sp = -2.f * (y*z + w*x);

	if (fabsf(sp) > .9999f)
	{
		p = M_PI_2 * sp;

		h = atan2f(-x*z - w*y, .5f - y*y - z*z);
		b = 0.0f;
	}
	else
	{
		p = asinf(sp);
		h = atan2f(x*z - w*y, .5f - x*x - y*y);
		b = atan2f(x*y - w*z, .5f - x*x - z*z);
	}

	pitch = EV1::RAD2DEG * p;
	heading = EV1::RAD2DEG * h;
	bank = EV1::RAD2DEG * b;
	
	return euler;
}

Rotator Quaternion::getRotator() const
{
	const float SingularityTest = z*x - w*y;
	const float YawY = 2.f*(w*z + x*y);
	const float YawX = (1.f - 2.f*(y*y + z*z));

	// reference 
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

	// this value was found from experience, the above websites recommend different values
	// but that isn't the case for us, so I went through different testing, and finally found the case 
	// where both of world lives happily. 
	const float SINGULARITY_THRESHOLD = 0.4999995f;
	Rotator RotatorFromQuat;

	if (SingularityTest < -SINGULARITY_THRESHOLD)
	{
		RotatorFromQuat.pitch = -90.f;
		RotatorFromQuat.yaw = atan2f(YawY, YawX) * EV1::RAD2DEG;
		RotatorFromQuat.roll = Rotator::NormalizeAxis(-RotatorFromQuat.yaw - (2.f * atan2f(x, w) * EV1::RAD2DEG));
	}
	else if (SingularityTest > SINGULARITY_THRESHOLD)
	{
		RotatorFromQuat.pitch = 90.f;
		RotatorFromQuat.yaw = atan2f(YawY, YawX) * EV1::RAD2DEG;
		RotatorFromQuat.roll = Rotator::NormalizeAxis(RotatorFromQuat.yaw - (2.f * atan2f(x, w) * EV1::RAD2DEG));
	}
	else
	{
		RotatorFromQuat.pitch = asinf(2.f*(SingularityTest)) * EV1::RAD2DEG;
		RotatorFromQuat.yaw = atan2f(YawY, YawX) * EV1::RAD2DEG;
		RotatorFromQuat.roll = atan2f(-2.f*(w*x + y*z), (1.f - 2.f*(x*x + y*y))) * EV1::RAD2DEG;
	}

	return RotatorFromQuat;
}

//////////////////////////////////////////////////////////////////////////
void Quaternion::Normalize()
{
	float InvS = inverse_f(calcLengthSquared());
	x *= InvS;
	y *= InvS;
	z *= InvS;
	w *= InvS;
}

float Quaternion::calcLength() const
{
	return sqrtf(calcLengthSquared());
}

float Quaternion::calcLengthSquared() const
{
	return (x * x) + (y * y) + (z * z) + (w * w);
}

void Quaternion::operator*=(const Quaternion& rightHandSide)
{
	Quaternion temp =*this * rightHandSide;
	x = temp.x;
	y = temp.y;
	z = temp.z;
	w = temp.w;
}

const Quaternion Quaternion::operator*(const Quaternion& rightHandSide)
{
	//*/
	Quaternion result;
	result.x =  x * rightHandSide.w +y * rightHandSide.z - z * rightHandSide.y + w * rightHandSide.x;
	result.y = -x * rightHandSide.z +y * rightHandSide.w + z * rightHandSide.x + w * rightHandSide.y;
	result.z =  x * rightHandSide.y -y * rightHandSide.x + z * rightHandSide.w + w * rightHandSide.z;
	result.w = -x * rightHandSide.x -y * rightHandSide.y - z * rightHandSide.z + w * rightHandSide.w;
	return result;
	/*/
	float leftW, rightW;
	Vector3 leftV, rightV;
	leftW = this->w;
	rightW = rightHandSide.w;
	leftV = Vector3(x, y, z);
	rightV = Vector3(rightHandSide.x, rightHandSide.y, rightHandSide.z);

	float resultW = leftW*rightW - dotProduct(leftV, rightV);
	Vector3 resultV = leftW * rightV + rightW * leftV + crossProduct(rightV, leftV);

	Quaternion result;
	result.w = resultW;
	result.x = resultV.x;
	result.y = resultV.y;
	result.z = resultV.z;
	return result;
	//*/
}


Vector3 Quaternion::getRotationAxis() const
{
	// Ensure we never try to sqrt a neg number
	const float S = sqrtf(EV1::Max(1.f - (w * w), 0.f));

	if (S >= 0.0001f)
	{
		const float InvS = inverse_f(S);
		return Vector3(x * InvS, y * InvS, z * InvS);
	}

	return Vector3(1.f, 0.f, 0.f);
}

void Quaternion::getAxisAndAngle(Vector3& axis, float& angle) const
{
	angle = EV1::RAD2DEG * 2.f * acosf(w);
	axis = getRotationAxis();
}

Quaternion Quaternion::CreateRotateObjectToInertial(const Vector3& euler)
{
	float sp, sb, sh;
	float cp, cb, ch;
	UE4::SinCos(sp, cp, euler.z *.5f);
	UE4::SinCos(sb, cb, euler.x *.5f);
	UE4::SinCos(sh, ch, euler.y *.5f);

	Quaternion result;
	result.w = ch*cp*cb + sh*sp*sb;
	result.x = ch*sp*cb + sh*cp*sb;
	result.y = -ch*sp*sb + sh*cp*cb;
	result.z = -sh*sp*cb + ch*cp*sb;
	return result;
}

Quaternion Quaternion::CreateRotateInertialToObject(const Vector3& euler)
{
	float sp, sb, sh;
	float cp, cb, ch;
	UE4::SinCos(sp, cp, euler.z *.5f);
	UE4::SinCos(sb, cb, euler.x *.5f);
	UE4::SinCos(sh, ch, euler.y *.5f);

	Quaternion result;
	result.w = ch*cp*cb + sh*sp*sb;
	result.x = -ch*sp*cb - sh*cp*sb;
	result.y = ch*sp*sb - sh*cp*cb;
	result.z = sh*sp*cb - ch*cp*sb;
	return result;
}

Quaternion Quaternion::CreateFromAxisAndAngleInRads(const Vector3& axis, float angleRadians)
{
	Quaternion result;
	const float halfAngle = .5f * angleRadians;
	float s, c;

	UE4::SinCos(s, c, halfAngle);

	result.x = s * axis.x;
	result.y = s * axis.y;
	result.z = s * axis.z;
	result.w = c;
	return result;
}

const Quaternion interpolate(const Quaternion& start, const Quaternion& end, float fractionToEnd)
{
	Quaternion result = slerp_notNormalized(start, end, fractionToEnd);
	result.Normalize();
	return result;
}

const Quaternion slerp_notNormalized(const Quaternion& start, const Quaternion& end, float fractionToEnd)
{
	// From UE4 because Quaterions are weird.

	// Get cosine of angle between quats.
	const float RawCosom =
		start.x * end.x +
		start.y * end.y +
		start.z * end.z +
		start.w * end.w;
	// Unaligned quats - compensate, results in taking shorter route.
	const float Cosom = FloatSelect(RawCosom, RawCosom, -RawCosom);

	float Scale0, Scale1;

	if (Cosom < 0.9999f)
	{
		const float Omega = qAcos(Cosom);
		const float InvSin = 1.f / sinf(Omega);
		Scale0 = sinf((1.f - fractionToEnd) * Omega) * InvSin;
		Scale1 = sinf(fractionToEnd * Omega) * InvSin;
	}
	else
	{
		// Use linear interpolation.
		Scale0 = 1.0f - fractionToEnd;
		Scale1 = fractionToEnd;
	}

	// In keeping with our flipped Cosom:
	Scale1 = FloatSelect(RawCosom, Scale1, -Scale1);

	Quaternion Result;

	Result.x = Scale0 * start.x + Scale1 * end.x;
	Result.y = Scale0 * start.y + Scale1 * end.y;
	Result.z = Scale0 * start.z + Scale1 * end.z;
	Result.w = Scale0 * start.w + Scale1 * end.w;

	return Result;
}

COMMAND(quat_euler_test, "angle vaules you want for each direction.")
{
	std::vector<std::string> args;
	splitArgs(arguments, args);
	Vector3 euler(stof(args[0]), stof(args[1]), stof(args[2]));
	
	Quaternion qtest;
	qtest.SetFromRotator(Rotator::MakeFromEuler(euler));
	
	Vector3 result = qtest.getEulerAngles();
	CONSOLE->addMessage(vectorToString(result));
}

//Local Test Function
void EulerQuatCheck(const Vector3& euler, uint attempt = 0)
{
	Matrix4 originalMat = Matrix4::CreateFromEuler(euler);
	
	Quaternion qtest;
	qtest.SetFromEuler(euler);
	Vector3 qeuler = qtest.getEulerAngles();
	Matrix4 qMat = Matrix4::CreateFromEuler(qeuler);

	float dotI = dotProduct(originalMat.getIBasis(), qMat.getIBasis());
	ASSERT_OR_DIE(dotI >= .99f, C_STRF("Euler: %s, did not match at IBasis. Attempts = %i", vectorToString(euler).c_str(), attempt));

	float dotJ = dotProduct(originalMat.getJBasis(), qMat.getJBasis());
	ASSERT_OR_DIE(dotJ >= .99f, C_STRF("Euler: %s, did not match at JBasis. Attempts = %i", vectorToString(euler).c_str(), attempt));
	
	float dotK = dotProduct(originalMat.getKBasis(), qMat.getKBasis());
	ASSERT_OR_DIE(dotK >= .99f, C_STRF("Euler: %s, did not match at KBasis. Attempts = %i", vectorToString(euler).c_str(), attempt));
}

Vector3 getRandomEuler()
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

COMMAND(quat_euler_test_auto, "Automatic test for converting Eulers to Quats")
{
	Vector3 original;
	//float& o_x = original.x;
	//float& o_y = original.y;
	//float& o_z = original.z;
	//Start with Obvious angles
	{

	}

	for (uint i = 0; i < 3000; i++)
	{
		original = getRandomEuler();
		EulerQuatCheck(original, i);
	}
}