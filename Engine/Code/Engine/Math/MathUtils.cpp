#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>
#include <stdlib.h>

//-
// Required for A1
int getRandomIntLessThan(int maxValueNotInclusive)
{
	if (maxValueNotInclusive == 0)
		return 0;
	return rand() % maxValueNotInclusive;
}

int getRandomIntInRange(int minValueInclusive, int maxValueInclusive)
{
	int numPossiblities = maxValueInclusive - minValueInclusive + 1;
	return minValueInclusive + getRandomIntLessThan(numPossiblities);
}

float getRandomFloatZeroToOne()
{
	return convertToFloat(rand()) / convertToFloat(RAND_MAX);
}

float getRandomFloatLessThan(float maxValueInclusive)
{
	return getRandomFloatZeroToOne()*maxValueInclusive;
}

float getRandomFloatInRange(float minimumInclusive, float maximumInclusive)
{
	return getRandomFloatLessThan(maximumInclusive - minimumInclusive) + minimumInclusive;
}

int getRandomNegativeOrPositiveOne()
{
	if (rand() % 2 == 0)
		return -1;
	else
		return 1;
}

void turnToward(float& currentDegrees, float goalDegrees, float maxTurnDegrees)
{
	float signedDist = calcShortestAngularDistance(currentDegrees, goalDegrees);

	if (fabs(signedDist) <= maxTurnDegrees)
		currentDegrees = goalDegrees;
	else if (signedDist > 0)
		currentDegrees += maxTurnDegrees;
	else
		currentDegrees -= maxTurnDegrees;
}

float calcShortestAngularDistance(float startDegress, float endDegrees)
{
	float angularDist = endDegrees - startDegress;

	while (angularDist > 180.f)
		angularDist -= 360.f;
	while (angularDist < -180.f)
		angularDist += 360.f;
	
	return angularDist;
}

//////////////////////////////////////////////////////////////////////////
// Bonus Stuff For Me
bool isPercentChance(float probability)
{
	float roll = getRandomFloatLessThan(1.f);
	return (roll < probability);
}

float cosDegrees(float degrees)
{
	float radians = convertDegreesToRadians(degrees);
	return cosf(radians);
}

float sinDegrees(float degrees)
{
	float radians = convertDegreesToRadians(degrees);
	return sinf(radians);
}

float atan2Degrees(float y, float x)
{
	float degrees = convertRadiansToDegrees(atan2f(y, x));
	return degrees;
}

float inverse_f(float divisor)
{
	GUARANTEE_OR_DIE(divisor != 0, "Attempted to divide by 0!");
	return 1.f / divisor;
}

double inverse_d(double divisor)
{
	GUARANTEE_OR_DIE(divisor != 0, "Attempted to divide by 0!");
	return 1.0 / divisor; 
}


const int SMALLEST_NON_DENORMAL = 8388608;
const float FLT_SMALLEST_NON_DENORMAL = *reinterpret_cast< const float * >(&SMALLEST_NON_DENORMAL);	// 1.1754944e-038f  from Id
float inverseSqrt_f(float divisor)
{
	return (divisor > FLT_SMALLEST_NON_DENORMAL) ? sqrtf(inverse_f(divisor)) : 1e30f;
}

float smoothStart(float t, int power)
{
	float x = 1;
	for (int i = 0; i < power; i++)
	{
		x *= t;
	}

	return x;
}

float smoothStop(float t, int power)
{
	float x = 1;
	for (int i = 0; i < power; i++)
	{
		x *= (1 - t);
	}

	return (1 - x);
}

float SmoothStep3(float t)
{

		float x = smoothStart(t, 3);

		float y = smoothStop(t, 3);

		return interpolate(x, y, .5);
}

float interpolate(float start, float end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	float blended;
	blended = (fractionOfStart * start) + (fractionToEnd * end);
	return blended;
}


float UE4::FloatMod(float X, float Y)
{
	if (fabsf(Y) <= 1.e-8f)
	{
		return 0.f;
	}
	const float Quotient = truncToFloat(X / Y);
	float IntPortion = Y * Quotient;

	// Rounding and imprecision could cause IntPortion to exceed X and cause the result to be outside the expected range.
	// For example Fmod(55.8, 9.3) would result in a very small negative value!
	if (fabsf(IntPortion) > fabsf(X))
	{
		IntPortion = X;
	}

	const float Result = X - IntPortion;
	return Result;
}
