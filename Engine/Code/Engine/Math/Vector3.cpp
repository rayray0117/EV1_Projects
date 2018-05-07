#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Matrix4.hpp"
#include "Quaternion.hpp"
//////////////////////////////////////////////////////////////////////////
const Vector3 Vector3::ZERO(0.f, 0.f, 0.f);
const Vector3 Vector3::ONE(1.f, 1.f, 1.f);
const Vector3 Vector3::XAXIS(1.f, 0.f, 0.f);
const Vector3 Vector3::YAXIS(0.f, 1.f, 0.f);
const Vector3 Vector3::ZAXIS(0.f, 0.f, 1.f);
//////////////////////////////////////////////////////////////////////////
// Constructors
Vector3::Vector3(const Vector2& copy, float Z /*= 0.f*/)
	: x(copy.x)
	, y(copy.y)
	, z(Z)
{
}

Vector3::Vector3(const IntVector3& copy)
	: x((float)copy.x)
	, y((float)copy.y)
	, z((float)copy.z)
{
}

//////////////////////////////////////////////////////////////////////////
const float* Vector3::getAsFloatArray() const
{
	return &x;
}

float* Vector3::getAsFloatArray()
{
	return &x;
}
//////////////////////////////////////////////////////////////////////////
// Accessors
void Vector3::getXYZ(float& outX, float& outY, float outZ) const
{
	outX = x;
	outY = y;
	outZ = z;
}

float Vector3::calcLength() const
{
	return sqrtf(calcLengthSquared());
}

Vector3 Vector3::getNormalizedVector() const
{
	return Vector3(x, y, z) / calcLength();
}
//////////////////////////////////////////////////////////////////////////
// Mutators
void Vector3::setXYZ(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

float Vector3::normalize()
{
	float length = calcLength();

	scaleUniform(inverse_f(length));
	return length;
}

float Vector3::setLength(float newLength)
{
	float initialLength = normalize();
	scaleUniform(newLength);
	return initialLength;
}

void Vector3::scaleUniform(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void Vector3::scaleNonUniform(const Vector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

void Vector3::inverseScaleNonUniform(const Vector3& perAxisScaleDivisors)
{
	x *= inverse_f(perAxisScaleDivisors.x);
	y *= inverse_f(perAxisScaleDivisors.y);
	z *= inverse_f(perAxisScaleDivisors.z);
}
//////////////////////////////////////////////////////////////////////////
// Operators
const Vector3 Vector3::operator/(float inverseScale)
{
	float scale = inverse_f(inverseScale);
	return Vector3(x * scale, y * scale, z * scale);
}


bool Vector3::isMostlyEqualTo(const Vector3& comparison, float epsilon /*= 0.001f*/) const
{
	float diffX = x - comparison.x;
	float diffY = y - comparison.y;
	float diffZ = z - comparison.z;
	return (((diffX < epsilon) && (-diffX < epsilon)) && ((diffY < epsilon) && (-diffY < epsilon)) && ((diffZ < epsilon) && (-diffZ < epsilon)));
}

float Vector3::getAngleBetweenDegrees(const Vector3& a, const Vector3& b)
{
	float angle = acos(dotProduct(a, b));
	return angle*EV1::RAD2DEG;
}

Vector3 Vector3::getAngleLimitedUnitVectorDegrees(const Vector3& vectorToLimit, const Vector3& baseline, float limit)
{
	float angleBetween = Vector3::getAngleBetweenDegrees(vectorToLimit, baseline);

	if (angleBetween > limit)
	{
		Vector3 correctionAxis = crossProduct(baseline.getNormalizedVector(), vectorToLimit.getNormalizedVector()).getNormalizedVector();
		return rotateAboutAxisDegrees(baseline, limit, correctionAxis).getNormalizedVector();
	}
	else
	{
		return vectorToLimit.getNormalizedVector();
	}
}

Vector3 Vector3::projectOntoPlane(const Vector3& planeNormal) const
{
	ASSERT_OR_DIE(planeNormal.calcLengthSquared() > 0.f, "Plane normal cannot be a zero vector.");

	Vector3 b = this->getNormalizedVector();
	Vector3 n = planeNormal.getNormalizedVector();
	Vector3 result = b - (n * dotProduct(b, planeNormal));
	result.normalize();
	return result;
}

Vector3 Vector3::rotateAboutAxisDegrees(const Vector3& vectorToRotate, float angleDegrees, const Vector3& rotationalAxis)
{
	return Quaternion(rotationalAxis, angleDegrees).getAsMatrix().TransformDirection(vectorToRotate.getNormalizedVector());
}

void Vector3::ToDirectionAndLength(Vector3& OutDir, float& OutLength)
{
	OutLength = calcLength();
	if (OutLength > (1.e-8f))
	{
		float OneOverLength = 1.0f / OutLength;
		OutDir = Vector3(y*OneOverLength, y*OneOverLength,
			z*OneOverLength);
	}
	else
	{
		OutDir = Vector3::ZERO;
	}
}

float Vector3::getSignedAngleBetweenDegs(Vector3 refVector, Vector3 otherVector, Vector3 normalVector)
{
	float usigned = getAngleBetweenDegrees(refVector, otherVector);
	float signVal = sign(dotProduct(crossProduct(refVector, otherVector), normalVector));
	return usigned * signVal;
}

//////////////////////////////////////////////////////////////////////////
// "friend" functions
float calcDistance(const Vector3& posA, const Vector3& posB)
{
	Vector3 displacement = posA - posB;
	return displacement.calcLength();
}

bool areMostlyEqual(const Vector3& a, const Vector3& b, float epsilon /*= 0.001f*/)
{
	float diffX = a.x - b.x;
	float diffY = a.y - b.y;
	float diffZ = a.z - b.z;
	return (((diffX < epsilon) && (-diffX < epsilon)) && ((diffY < epsilon) && (-diffY < epsilon)) && ((diffZ < epsilon) && (-diffZ < epsilon)));
}

const Vector3 interpolate(const Vector3& start, const Vector3& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	Vector3 blended;
	blended.x = (fractionOfStart * start.x) + (fractionToEnd * end.x);
	blended.y = (fractionOfStart * start.y) + (fractionToEnd * end.y);
	blended.z = (fractionOfStart * start.z) + (fractionToEnd * end.z);
	return blended;
}

const Vector3 crossProduct(const Vector3& a, const Vector3& b)
{
	float x = a.y * b.z - b.y * a.z;
	float y = a.z * b.x - b.z * a.x;
	float z = a.x * b.y - b.x * a.y;
	return Vector3(x, y, z);
}

const Vector3 getRandomVector3InRange(const Vector3& min, const Vector3& max)
{
	Vector3 result;

	result.x = getRandomFloatInRange(min.x, max.x);
	result.y = getRandomFloatInRange(min.y, max.y);
	result.z = getRandomFloatInRange(min.z, max.z);

	return result;
}

const Vector3 getDisplacement(const Vector3& from, const Vector3& to)
{
	return to - from;
}

const Vector3 getDirection(const Vector3& from, const Vector3& to)
{
	Vector3 displacement = getDisplacement(from, to);
	return displacement.getNormalizedVector();
}

