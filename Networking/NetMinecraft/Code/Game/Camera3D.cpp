#include "Game/Camera3D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
//////////////////////////////////////////////////////////////////////////
Camera3D::Camera3D()
	: m_position(Vector3::ZERO)
	, m_degreesAboutY(0.f)
	, m_degreesAboutX(0.f)
	, m_degreesAboutZ(0.f)
	, m_isFlying(false)
	, m_ZUp(true)
{
}

Camera3D::~Camera3D()
{
}
//////////////////////////////////////////////////////////////////////////
void Camera3D::Render() const
{
	//Setting up the Perspective for Z up
	g_renderer->SetPerspectiveProjection(g_fovVerticalDegrees, getAspectRatio(), g_nearClip, g_farClip);
	g_renderer->rotate3D(-90.f, Vector3::XAXIS);
	g_renderer->rotate3D(90.f, Vector3::ZAXIS);

	// Actual camera rotation stuff
	g_renderer->rotate3D(-m_degreesAboutX, Vector3::XAXIS);
	g_renderer->rotate3D(-m_degreesAboutY, Vector3::YAXIS);
	g_renderer->rotate3D(-m_degreesAboutZ, Vector3::ZAXIS);

	g_renderer->translate3D(m_position* -1.f);

	//g_renderer->SetEyePosition(m_position);
}
//////////////////////////////////////////////////////////////////////////
void Camera3D::UpdateCameraFromMouse(float deltaSeconds)
{
	if (!g_theApp->m_hasFocus)
		return;
	IntVector2 screenCenter = g_inputSystem->GetScreenCenter();
	IntVector2 mouseScreenPos = g_inputSystem->GetCursorScreenPos();
	IntVector2 mouseDeltaMove = mouseScreenPos - screenCenter;
	g_inputSystem->SetCursorScreenPos(screenCenter);

	float mouseMoveX = (float)mouseDeltaMove.x;
	rotateLeft(1.f, mouseMoveX*MOUSE_SENSITIVITY_X);
	float mouseMoveY = (float)mouseDeltaMove.y;
	rotateUp(1.f, mouseMoveY * MOUSE_SENSITIVITY_Y);


	bool isSprinting = g_inputSystem->isKeyDown(KEYCODE_SHIFT);

	if (g_inputSystem->isKeyDown('W'))
		moveForward(deltaSeconds, isSprinting);
	if (g_inputSystem->isKeyDown('S'))
		moveBackward(deltaSeconds, isSprinting);
	if (g_inputSystem->isKeyDown('A'))
		moveLeft(deltaSeconds, isSprinting);
	if (g_inputSystem->isKeyDown('D'))
		moveRight(deltaSeconds, isSprinting);
	if (g_inputSystem->isKeyDown(KEYCODE_SPACE))
		moveUp(deltaSeconds, isSprinting);//m_position += CAMERA_MOVEMENT_SPEED * deltaSeconds * Vector3::ZAXIS;
	if (g_inputSystem->isKeyDown(KEYCODE_CONTROL))
		moveDown(deltaSeconds, isSprinting); //m_position -= CAMERA_MOVEMENT_SPEED * deltaSeconds * Vector3::ZAXIS;

	if (g_inputSystem->isKeyDown(KEYCODE_UP))
		rotateUp(deltaSeconds, MOUSE_SENSITIVITY_Y);
	if (g_inputSystem->isKeyDown(KEYCODE_DOWN))
		rotateDown(deltaSeconds, MOUSE_SENSITIVITY_Y);

	if (g_inputSystem->isKeyDown(KEYCODE_LEFT))
		rotateLeft(deltaSeconds, MOUSE_SENSITIVITY_X);
	if (g_inputSystem->isKeyDown(KEYCODE_RIGHT))
		rotateRight(deltaSeconds, MOUSE_SENSITIVITY_X);
}
//////////////////////////////////////////////////////////////////////////
void Camera3D::moveForward(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction;
	if (m_isFlying)
	{
		if (m_ZUp)
		{
			direction = getForwardXYZ();
		}
		else
		{
			direction = getForwardXZY();
		}
	}
	else
	{
		if (m_ZUp)
		{
			direction = getForwardXY();
		}
		else
		{
			direction = getForwardXZ();
		}
	}

	movePosition(deltaSeconds, direction, isSprinting);
}

void Camera3D::moveBackward(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction;
	if (m_isFlying)
	{
		if (m_ZUp)
		{
			direction = getForwardXYZ();
		}
		else
		{
			direction = getForwardXZY();
		}
	}
	else
	{
		if (m_ZUp)
		{
			direction = getForwardXY();
		}
		else
		{
			direction = getForwardXZ();
		}
	}

	movePosition(deltaSeconds, -direction, isSprinting);
}

void Camera3D::moveLeft(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction;
	if (m_ZUp)
	{
		direction = getLeftXY();
	}
	else
	{
		direction = getLeftXZ();
	}

	movePosition(deltaSeconds, direction, isSprinting);
}

void Camera3D::moveRight(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction;
	if (m_ZUp)
	{
		direction = getLeftXY();
	}
	else
	{
		direction = getLeftXZ();
	}

	movePosition(deltaSeconds, -direction, isSprinting);
}

void Camera3D::moveUp(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction;
	if (m_ZUp)
	{
		direction = Vector3::ZAXIS;
	}
	else
	{
		direction = Vector3::YAXIS;
	}
	movePosition(deltaSeconds, direction, isSprinting);
}

void Camera3D::moveDown(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction;
	if (m_ZUp)
	{
		direction = Vector3::ZAXIS;
	}
	else
	{
		direction = Vector3::YAXIS;
	}
	movePosition(deltaSeconds, -direction, isSprinting);
}

//////////////////////////////////////////////////////////////////////////
Vector3 Camera3D::getForwardXYZ() const
{
	return Vector3(cosDegrees(m_degreesAboutZ)*cosDegrees(m_degreesAboutY), sinDegrees(m_degreesAboutZ)*cosDegrees(m_degreesAboutY), -sinDegrees(m_degreesAboutY));
}

Vector3 Camera3D::getForwardXY() const
{
	return Vector3(cosDegrees(m_degreesAboutZ), sinDegrees(m_degreesAboutZ), 0.f);
}

Vector3 Camera3D::getLeftXY() const
{
	Vector3 forward = getForwardXY();
	return Vector3(-forward.y, forward.x, 0.f);
}

Vector3 Camera3D::getForwardXZY() const
{
	//When Not Working yet
	return -Vector3(sinDegrees(m_degreesAboutY)*cosDegrees(m_degreesAboutX), -sinDegrees(m_degreesAboutX), cosDegrees(m_degreesAboutY)*cosDegrees(m_degreesAboutX));
}

Vector3 Camera3D::getForwardXZ() const
{
	//When X = forward
	return -Vector3(sinDegrees(m_degreesAboutY), 0.f, cosDegrees(m_degreesAboutY)); //Because our Z is -1 make this negative
}

Vector3 Camera3D::getLeftXZ() const
{
	Vector3 forward = getForwardXZ();
	return -Vector3(-forward.z, 0.f, forward.x);
}

//////////////////////////////////////////////////////////////////////////
void Camera3D::movePosition(float deltaSeconds, Vector3 direction, bool isSprinting /*= false*/)
{
	float speed = CAMERA_MOVEMENT_SPEED;
	if (isSprinting)
		speed *= CAMERA_SPRINT_MULTIPIER;

	m_position += speed * deltaSeconds * direction;
}

void Camera3D::movePositionController(float deltaSeconds, Vector3 stickdirection, float stickMagnitude, bool isSprinting /*= false*/)
{
	Vector3 direction;
	if (m_isFlying)
		direction = getForwardXYZ();
	else
		direction = getForwardXY();

	float speed = CAMERA_MOVEMENT_SPEED * stickMagnitude;
	if (isSprinting)
		speed *= CAMERA_SPRINT_MULTIPIER;

	m_position += speed * deltaSeconds * direction * stickdirection.y;

	direction = getLeftXY();


	m_position += speed * deltaSeconds * direction * stickdirection.x;
}

void Camera3D::rotateUp(float deltaSeconds, float sensitivity)
{
	if (m_ZUp)
	{
		m_degreesAboutY += sensitivity * deltaSeconds;
		m_degreesAboutY = clamp(m_degreesAboutY, -89.9f, 89.9f);
	}
	else
	{
		m_degreesAboutX -= sensitivity * deltaSeconds;
		m_degreesAboutX = clamp(m_degreesAboutX, -89.9f, 89.9f);
	}
}

void Camera3D::rotateDown(float deltaSeconds, float sensitivity)
{
	if (m_ZUp)
	{
		m_degreesAboutY -= sensitivity * deltaSeconds;
		m_degreesAboutY = clamp(m_degreesAboutY, -89.9f, 89.9f);
	}
	else
	{
		m_degreesAboutX += sensitivity * deltaSeconds;
		m_degreesAboutX = clamp(m_degreesAboutX, -89.9f, 89.9f);
	}
}

void Camera3D::rotateLeft(float deltaSeconds, float sensitivity)
{
	if (m_ZUp)
	{
		m_degreesAboutZ -= sensitivity * deltaSeconds;
	}
	else
	{
		m_degreesAboutY -= sensitivity * deltaSeconds;
	}
}

void Camera3D::rotateRight(float deltaSeconds, float sensitivity)
{
	if (m_ZUp)
	{
		m_degreesAboutZ += sensitivity * deltaSeconds;
	}
	else
	{
		m_degreesAboutY += sensitivity * deltaSeconds;
	}
}

