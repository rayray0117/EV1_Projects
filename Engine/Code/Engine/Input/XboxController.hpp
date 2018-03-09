#pragma once
#include "Engine/Math/Vector2.hpp"
//////////////////////////////////////////////////////////////////////////
enum XboxButton
{
	GAMEPAD_DPAD_UP	,
	GAMEPAD_DPAD_DOWN,
	GAMEPAD_DPAD_LEFT,
	GAMEPAD_DPAD_RIGHT,
	GAMEPAD_START,
	GAMEPAD_BACK,
	GAMEPAD_LEFT_THUMB	,
	GAMEPAD_RIGHT_THUMB	,
	GAMEPAD_LEFT_SHOULDER,
	GAMEPAD_RIGHT_SHOULDER,
	GAMEPAD_A,
	GAMEPAD_B,
	GAMEPAD_X,
	GAMEPAD_Y,
	NUM_GAMEPAD_BUTTONS
};
//////////////////////////////////////////////////////////////////////////
const int MAX_XBOX_CONTROLLERS		=  4;
const int XBOX_STICK_RAW_MIN		= -32768;
const int XBOX_STICK_RAW_MAX		=  32767;
const int XBOX_LEFT_STICK_DEADZONE	=  5000/*7849*/;
const int XBOX_RIGHT_STICK_DEADZONE =  8689;
const int XBOX_OUTER_STICK_DEADZONE =  31000;
const int XBOX_TRIGGER_THRESHOLD	=  30;
//////////////////////////////////////////////////////////////////////////
struct ButtonState
{
	bool m_isDown;
	bool m_justChanged;
};
//////////////////////////////////////////////////////////////////////////
struct XboxStick
{
	XboxStick()
		: m_position(Vector2::ZERO)
		, m_magnitude(0.f)
		, m_degrees(0.f)
	{}

	Vector2 m_position;
	float	m_magnitude;
	float	m_degrees;
	void UpdateFromRawPosition(short leftRawStickX, short leftRawStickY);
};
//////////////////////////////////////////////////////////////////////////
class XboxController
{
public:
	XboxController();
	~XboxController();

	void Update();

	bool isButtonDown(XboxButton buttonID) const;
	bool wasButtonJustPressed(XboxButton buttonID) const;
	bool wasButtonJustReleased(XboxButton buttonID) const;
private:
	void UpdateButton(XboxButton buttonID, int buttonBitValue, unsigned short buttonFlag);
	
public:
	int m_controllerID;
	bool m_isConnected;
	XboxStick	m_leftStick;
	XboxStick	m_rightStick;
	ButtonState m_buttonStates[NUM_GAMEPAD_BUTTONS];
	float		m_leftTrigger;
	float		m_rightTrigger;
};