#pragma once
//////////////////////////////////////////////////////////////////////////
class Rgba;
class Vector3;
//////////////////////////////////////////////////////////////////////////

class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	static const Vector4 ZERO;
	static const Vector4 ONE;
public:
	Vector4();
	Vector4(const Vector4& copy);
	explicit Vector4(float initialX, float initialY, float initialZ, float initialW);
	Vector4(const Rgba& colors);
	Vector4(const Rgba& colors, float intensity);
	Vector4(const Vector3& copy, float _w = 1.0f);

	const float* getAsFloatArray() const;
	float* getAsFloatArray();

	// Accessors
	void getXYZW(float& outX, float& outY, float& outZ, float& outW) const;
	float calcLength3D() const;
	float calcLengthSquared3D() const;
	float calcLength4D() const;
	float calcLengthSquared4D() const;

	// Mutators
	void setXYZW(float newX, float newY, float newZ, float newW);
	float normalize3D();
	float normalize4D();
	void scaleUniform(float scale);
	void scaleNonUniform(const Vector4& perAxisScaleFactors);
	void inverseScaleNonUniform(const Vector4& perAxisScaleDivisors);

	// Operators
	bool operator==(const Vector4& rightHandSide) const;
	bool operator!=(const Vector4& rightHandSide) const;
	const Vector4 operator+(const Vector4& rightHandSide) const;
	const Vector4 operator-(const Vector4& rightHandSide) const;
	const Vector4 operator-() const;
	const Vector4 operator*(float scale) const;
	const Vector4 operator*(const Vector4& perAxisScaleFactors) const;
	const Vector4 operator/(float inverseScale);
	void operator*=(float scale);
	void operator*=(const Vector4& perAxisScaleFactors);
	void operator+=(const Vector4& rightHandSide);
	void operator-=(const Vector4& rightHandSide);

	float	operator[](int index) const;
	float&	operator[](int index);

	bool isMostlyEqualTo(const Vector4& comparison, float epsilon = 0.001f);

	//Standalone friend helper functions
	friend float calcDistance(const Vector4& posA, const Vector4& posB);
	friend float calcDistanceSquared(const Vector4& posA, const Vector4& posB);
	friend const Vector4 operator*(float scale, const Vector4& vectorToScale);
	friend bool areMostlyEqual(const Vector4& a, const Vector4& b, float epsilon = 0.001f);
	friend float dotProduct(const Vector4& a, const Vector4& b);
	friend const Vector4 interpolate(const Vector4& start, const Vector4& end, float fractionToEnd);

	float MaxAbsoluteValue();
};

//////////////////////////////////////////////////////////////////////////
// Inlines

inline const Vector4 Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}

inline float Vector4::operator[](int index) const
{
	return (&x)[index];
}
inline float& Vector4::operator[](int index)
{
	return (&x)[index];
}