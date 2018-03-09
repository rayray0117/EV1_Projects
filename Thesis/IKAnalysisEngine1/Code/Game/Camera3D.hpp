#pragma once
#pragma once
#include "Engine/Math/Vector3.hpp"

class Camera3D
{
public:
	Camera3D();
	~Camera3D();

	void moveForward(float deltaSeconds, bool isSprinting = false);
	void moveBackward(float deltaSeconds, bool isSprinting = false);
	void moveLeft(float deltaSeconds, bool isSprinting = false);
	void moveRight(float deltaSeconds, bool isSprinting = false);
	void moveUp(float deltaSeconds, bool isSprinting = false);
	void moveDown(float deltaSeconds, bool isSprinting = false);

	void movePositionController(float deltaSeconds, Vector3 stickdirection, float stickMagnitude, bool isSprinting = false);

	void rotateUp(float deltaSeconds, float sensitivity);
	void rotateDown(float deltaSeconds, float sensitivity);
	void rotateLeft(float deltaSeconds, float sensitivity);
	void rotateRight(float deltaSeconds, float sensitivity);

	Vector3 getForwardXYZ() const;
	Vector3 getForwardXY() const;
	Vector3 getLeftXY() const;
	Vector3 getForwardXZY() const;
	Vector3 getForwardXZ() const;
	Vector3 getLeftXZ() const;
	/*Vector3 getLeftXYZ() const;*/

public:
	Vector3 m_position;
	float m_degreesAboutX;
	float m_degreesAboutY;
	float m_degreesAboutZ;
	bool m_isFlying;
	bool m_ZUp;

private:
	void movePosition(float deltaSeconds, Vector3 direction, bool isSprinting = false);
};
