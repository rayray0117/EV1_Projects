#pragma once
//////////////////////////////////////////////////////////////////////////
class IntVector3;
class Vector2;
//////////////////////////////////////////////////////////////////////////
class Vector3
{
public:
	float x;
	float y;
	float z;

	static const Vector3 ZERO;
	static const Vector3 ONE;
	static const Vector3 XAXIS;
	static const Vector3 YAXIS;
	static const Vector3 ZAXIS;

	static float getSignedAngleBetweenDegs(Vector3 relativeHingeReferenceAxis, Vector3 thisBoneInnerToOuterUV, Vector3 relativeHingeRotationAxis);
public:
	Vector3();
	Vector3(const Vector3& copy);
	explicit Vector3(float initialX, float initialY, float initialZ);
	Vector3(const Vector2& copy, float Z = 0.f);
	Vector3(const IntVector3& copy);

	const float* getAsFloatArray() const;
	float* getAsFloatArray();

	//Accessors
	void getXYZ(float& outX, float& outY, float outZ) const;
	float calcLength() const;
	float calcLengthSquared() const;
	Vector3 getNormalizedVector() const;

	//Mutators
	void setXYZ(float newX, float newY, float newZ);
	float normalize(); // Will return the vector's initial length
	float setLength(float newLength); // Will return the vector's initial length
	void scaleUniform(float scale);
	void scaleNonUniform(const Vector3& perAxisScaleFactors);
	void inverseScaleNonUniform(const Vector3& perAxisScaleDivisors);

	//Operators
	bool operator==(const Vector3& rightHandSide) const;
	bool operator!=(const Vector3& rightHandSide) const;
	const Vector3 operator+(const Vector3& rightHandSide) const;
	const Vector3 operator-(const Vector3& rightHandSide) const;
	const Vector3 operator-() const;
	const Vector3 operator*(float scale) const;
	const Vector3 operator*(const Vector3& perAxisScaleFactors) const;
	const Vector3 operator/(float inverseScale);
	void operator*=(float scale);
	void operator*=(const Vector3& perAxisScaleFactors);
	void operator+=(const Vector3& rightHandSide);
	void operator-=(const Vector3& rightHandSide);

	bool isMostlyEqualTo(const Vector3& comparison, float epsilon = 0.001f) const;

	//Standalone friend helper functions
	friend float calcDistance(const Vector3& posA, const Vector3& posB);
	friend float calcDistanceSquared(const Vector3& posA, const Vector3& posB);
	friend const Vector3 operator*(float scale, const Vector3& vectorToScale);
	friend bool areMostlyEqual(const Vector3& a, const Vector3& b, float epsilon = 0.001f);
	friend float dotProduct(const Vector3& a, const Vector3& b);
	friend const Vector3 interpolate(const Vector3& start, const Vector3& end, float fractionToEnd);
	friend const Vector3 crossProduct(const Vector3& a, const Vector3& b);
	friend const Vector3 getDisplacement(const Vector3& from, const Vector3& to);
	friend const Vector3 getDirection(const Vector3& from, const Vector3& to);

	friend const Vector3 getRandomVector3InRange(const Vector3& min, const Vector3& max);
	static float getAngleBetweenDegrees(const Vector3& a, const Vector3& b);
	static Vector3 getAngleLimitedUnitVectorDegrees(const Vector3& a, const Vector3& b, float limit);
	Vector3 projectOntoPlane(const Vector3& planeNormal) const;
	static Vector3 rotateAboutAxisDegrees(const Vector3& source, float angleDegrees, const Vector3& rotationalAxis);
	void ToDirectionAndLength(Vector3& EndEffectorToTargetDir, float& EndEffectToTargetSize);
};

//////////////////////////////////////////////////////////////////////////
// Inline Functions
//////////////////////////////////////////////////////////////////////////
// Constructors
inline Vector3::Vector3()
{
}

inline Vector3::Vector3(const Vector3& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
{
}

inline Vector3::Vector3(float initialX, float initialY, float initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}

//////////////////////////////////////////////////////////////////////////
// Accessors
inline float Vector3::calcLengthSquared() const
{
	return (x * x) + (y * y) + (z * z);
}
//////////////////////////////////////////////////////////////////////////
// Operators
inline bool Vector3::operator==(const Vector3& rightHandSide) const
{
	return ((x == rightHandSide.x) && (y == rightHandSide.y) && (z == rightHandSide.z));
}

inline bool Vector3::operator!=(const Vector3& rightHandSide) const
{
	return((x != rightHandSide.x) || (y != rightHandSide.y) || (z != rightHandSide.z)); // equivalent to !((x == rightHandSide.x) && (y == rightHandSide.y) && (z == rightHandSide.z));
}

inline const Vector3 Vector3::operator+(const Vector3& rightHandSide) const
{
	return Vector3(x + rightHandSide.x, y + rightHandSide.y, z + rightHandSide.z);
}

inline const Vector3 Vector3::operator-(const Vector3& rightHandSide) const
{
	return Vector3(x - rightHandSide.x, y - rightHandSide.y, z - rightHandSide.z);
}

inline const Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

inline const Vector3 Vector3::operator*(float scale) const
{
	return Vector3(x * scale, y * scale, z * scale);
}

inline const Vector3 Vector3::operator*(const Vector3& perAxisScaleFactors) const
{
	return Vector3(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z);
}

inline void Vector3::operator*=(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

inline void Vector3::operator*=(const Vector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

inline void Vector3::operator+=(const Vector3& rightHandSide)
{
	x += rightHandSide.x;
	y += rightHandSide.y;
	z += rightHandSide.z;
}

inline void Vector3::operator-=(const Vector3& rightHandSide)
{
	x -= rightHandSide.x;
	y -= rightHandSide.y;
	z -= rightHandSide.z;
}

//////////////////////////////////////////////////////////////////////////
inline float calcDistanceSquared(const Vector3& posA, const Vector3& posB)
{
	Vector3 displacement = posA - posB;
	return displacement.calcLengthSquared();
}

inline const Vector3 operator*(float scale, const Vector3& vectorToScale)
{
	return vectorToScale * scale;
}

inline float dotProduct(const Vector3& a, const Vector3& b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}