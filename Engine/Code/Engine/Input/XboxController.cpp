#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/MathUtils.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#include "Engine/Core/ErrorWarningAssert.hpp"
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; using 9_1_0 explicitly

XboxController::XboxController()
	: m_isConnected(false)
	, m_controllerID(-1)
	, m_leftTrigger(0.f)
	, m_rightTrigger(0.f)
{
	for (int buttonIndex = 0; buttonIndex < NUM_GAMEPAD_BUTTONS; buttonIndex++)
	{
		m_buttonStates[buttonIndex].m_isDown = false;
		m_buttonStates[buttonIndex].m_justChanged = false;
	}
}

XboxController::~XboxController()
{

}

void XboxController::Update()
{
	XINPUT_STATE xboxControllerState;
	memset(&xboxControllerState, 0, sizeof(xboxControllerState));
	DWORD errorStatus = XInputGetState(m_controllerID, &xboxControllerState);
	if (errorStatus == ERROR_DEVICE_NOT_CONNECTED)
	{
		m_isConnected = false;
		//DebuggerPrintf("Xbox Controller %i is not connected \n", m_controllerID);
		return;
	}

	m_isConnected = true;
	
	// Update joy stick positions (in
	short leftRawStickX = xboxControllerState.Gamepad.sThumbLX;
	short leftRawStickY = xboxControllerState.Gamepad.sThumbLY;
	short rightRawStickX = xboxControllerState.Gamepad.sThumbRX;
	short rightRawStickY = xboxControllerState.Gamepad.sThumbRY;

	//DebuggerPrintf("Left stick is at x,y = (%i,%i)\n", leftRawStickX, leftRawStickY);
	m_leftStick.UpdateFromRawPosition(leftRawStickX, leftRawStickY);
	m_rightStick.UpdateFromRawPosition(rightRawStickX,rightRawStickY);

		
	// update trigger
	unsigned char leftTriggerRaw = xboxControllerState.Gamepad.bLeftTrigger;
	unsigned char rightTriggerRaw = xboxControllerState.Gamepad.bRightTrigger;
	m_leftTrigger = rangeMapFloat(convertToFloat(leftTriggerRaw), 0.f, 255.f, 0.f, 1.f);
	m_rightTrigger = rangeMapFloat(convertToFloat(rightTriggerRaw), 0.f, 255.f, 0.f, 1.f);

	// update buttons
	unsigned short buttonsFlags = xboxControllerState.Gamepad.wButtons;
	UpdateButton(GAMEPAD_DPAD_UP, 0x0001, buttonsFlags);
	UpdateButton(GAMEPAD_DPAD_DOWN, 0x0002, buttonsFlags);
	UpdateButton(GAMEPAD_DPAD_LEFT, 0x0004, buttonsFlags);
	UpdateButton(GAMEPAD_DPAD_RIGHT, 0x0008, buttonsFlags);
	UpdateButton(GAMEPAD_START, 0x0010, buttonsFlags);
	UpdateButton(GAMEPAD_BACK, 0x0020, buttonsFlags);
	UpdateButton(GAMEPAD_LEFT_THUMB, 0x0040, buttonsFlags);
	UpdateButton(GAMEPAD_RIGHT_THUMB, 0x0080, buttonsFlags);
	UpdateButton(GAMEPAD_LEFT_SHOULDER, 0x0100, buttonsFlags);
	UpdateButton(GAMEPAD_RIGHT_SHOULDER, 0x0200, buttonsFlags);
	UpdateButton(GAMEPAD_A, 0x1000, buttonsFlags);
	UpdateButton(GAMEPAD_B, 0x2000, buttonsFlags);
	UpdateButton(GAMEPAD_X, 0x4000, buttonsFlags);
	UpdateButton(GAMEPAD_Y, 0x8000, buttonsFlags);
}

bool XboxController::isButtonDown(XboxButton buttonID) const
{
	return m_buttonStates[buttonID].m_isDown;
}

bool XboxController::wasButtonJustPressed(XboxButton buttonID) const
{
	return isButtonDown(buttonID) && m_buttonStates[buttonID].m_justChanged;
}

bool XboxController::wasButtonJustReleased(XboxButton buttonID) const
{
	return !isButtonDown(buttonID) && m_buttonStates[buttonID].m_justChanged;
}

void XboxController::UpdateButton(XboxButton buttonID, int buttonBitValue, unsigned short buttonFlag)
{
	if ((buttonBitValue & buttonFlag) > 0)
	{
		m_buttonStates[buttonID].m_justChanged = !m_buttonStates[buttonID].m_isDown;
		m_buttonStates[buttonID].m_isDown = true;
	}
	else
	{
		m_buttonStates[buttonID].m_justChanged = m_buttonStates[buttonID].m_isDown;
		m_buttonStates[buttonID].m_isDown = false;
	}
}

void XboxStick::UpdateFromRawPosition(short rawX, short rawY)
{
	float unitX = rangeMapFloat(convertToFloat(rawX), convertToFloat(XBOX_STICK_RAW_MIN), convertToFloat(XBOX_STICK_RAW_MAX), -1.f, 1.f);
	float unitY = rangeMapFloat(convertToFloat(rawY), convertToFloat(XBOX_STICK_RAW_MIN), convertToFloat(XBOX_STICK_RAW_MAX), -1.f, 1.f);
	//DebuggerPrintf("unitX = %f, unitY = %f", unitX, unitY);
	float innerDeadZone = rangeMapFloat(convertToFloat(XBOX_LEFT_STICK_DEADZONE), convertToFloat(XBOX_STICK_RAW_MIN), convertToFloat(XBOX_STICK_RAW_MAX), -1.f, 1.f);
	float outerDeadZone = rangeMapFloat(convertToFloat(XBOX_OUTER_STICK_DEADZONE), convertToFloat(XBOX_STICK_RAW_MIN), convertToFloat(XBOX_STICK_RAW_MAX), -1.f, 1.f);

	float degrees = atan2Degrees(unitY, unitX);
	float rawMagnitude = calcDistance(Vector2::ZERO, Vector2(unitX, unitY));

	float correctedMagnitude = rangeMapFloat(rawMagnitude, convertToFloat(innerDeadZone), convertToFloat(outerDeadZone), -1.f, 1.f);
	correctedMagnitude = clamp(correctedMagnitude, 0.f, 1.f);

	m_magnitude = correctedMagnitude;
	m_degrees = degrees;
	m_position.setLengthAndHeadingDegrees(m_magnitude, m_degrees);
}
