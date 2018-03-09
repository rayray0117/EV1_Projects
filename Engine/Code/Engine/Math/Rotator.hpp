#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/Vector3.hpp"
//#include "Engine/Math/Quaternion.hpp"
// Idea from UE4 and recommended by Forseth.
//////////////////////////////////////////////////////////////////////////
class Quaternion;
//////////////////////////////////////////////////////////////////////////
class Rotator
{
public:
	float pitch; //aka Bank. Rotation around the right axis (around x axis), Looking up and down (0=Straight Ahead, +Up, -Down). 
	float yaw;   //aka Heading. Rotation around the up axis (around y axis), Running in circles 0=East, +North, -South. // 0Deg = 1X. 0Y. 90Deg = 0X 1Y.
	float roll;  //aka Attitude. Rotation around the forward axis (around z axis), Tilting your head, 0=Straight, -Clockwise, +CCW.

public:
	FORCEINLINE Rotator() {}
	explicit Rotator(float inPitch, float inYaw, float inRoll) : pitch(inPitch), yaw(inYaw), roll(inRoll){}
	explicit Rotator(const Quaternion& quat);
	
	Vector3		getDirection() const; //Get a unit vector in the direction the vector is pointing torward.
	Vector3		getAsVector() const; //Returns a Vector3 that holds the angels in degrees.
	//Quaternion	getAsQuat() const; 


	Rotator getClamped() const; // Gets the rotation values so they fall within the range [0,360]
	Rotator getNormalized() const; // Create a copy of this rotator and normalize, removes all winding and creates the "shortest route" rotation.
	Rotator getDenormalized() const; // Create a copy of this rotator and denormalize, clamping each axis to 0 - 360.


	void Normalize(); // In-place normalize, removes all winding and creates the "shortest route" rotation.
	
	static Vector3 AxisAngleToEuler(const Vector3& axis, float angleRads);
	//*/static void EulerToAxisAngle(const Vector3& euler,Vector3& axis, float& angleDegrees);

	/**
	* Clamps an angle to the range of [0, 360).
	*
	* @param Angle The angle to clamp.
	* @return The clamped angle.
	*/
	static float ClampAxis(float Angle);

	/**
	* Clamps an angle to the range of (-180, 180].
	*
	* @param Angle The Angle to clamp.
	* @return The clamped angle.
	*/
	static float NormalizeAxis(float Angle);

	static Rotator MakeFromEuler(const Vector3& euler);
	static Rotator MakeFromDirection(const Vector3& dir);
};