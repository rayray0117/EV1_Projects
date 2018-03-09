#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


InputSystem::InputSystem()
{
	for (int keyIndex = 0; keyIndex < NUM_KEYBOARD_KEYS; keyIndex++)
	{
		m_keyStates[keyIndex].m_isDown = false;
		m_keyStates[keyIndex].m_justChanged = false;
	}

 	for (int controllerIndex = 0; controllerIndex < MAX_XBOX_CONTROLLERS; ++controllerIndex)
 	{
		m_xboxControllers[controllerIndex].m_controllerID = controllerIndex;
	}
}

InputSystem::~InputSystem()
{
}

void InputSystem::BeginFrame()
{
 	for (int XboxControllerindex = 0; XboxControllerindex < MAX_XBOX_CONTROLLERS; ++XboxControllerindex)
 	{
		m_xboxControllers[XboxControllerindex].Update();
	}
	//DebuggerPrintf("Magnitude = %f, Degrees = %f, Position = (%f,%f). \n", m_xboxControllers[0].m_leftStick.m_magnitude, m_xboxControllers[0].m_leftStick.m_degrees, m_xboxControllers[0].m_leftStick.m_position.x, m_xboxControllers[0].m_leftStick.m_position.y);
	for (int keyIndex = 0; keyIndex < NUM_KEYBOARD_KEYS; ++keyIndex)
	{
		m_keyStates[keyIndex].m_wasJustPressed = false;
		m_keyStates[keyIndex].m_wasJustReleased = false;
	}
}

void InputSystem::EndFrame()
{
	for (int keyIndex = 0; keyIndex < NUM_KEYBOARD_KEYS; ++keyIndex)
	{
		m_keyStates[keyIndex].m_justChanged = false;
	}
}

void InputSystem::Render() const
{
	// ToDo: draw debug overlay of keyboard & xbox states when f11 is pressed
}

void InputSystem::OnKeyPressed(unsigned char keyIndex)
{
	KeyState& keyState = m_keyStates[keyIndex];

	bool wasDown = keyState.m_isDown;
	bool isDown = true;
	bool justChanged = (wasDown != isDown);
	keyState.m_wasJustPressed = true;

	keyState.m_isDown = isDown;
	keyState.m_justChanged = justChanged;
}

void InputSystem::OnKeyReleased(unsigned char keyIndex)
{
	KeyState& keyState = m_keyStates[keyIndex];

	bool wasDown = keyState.m_isDown;
	bool isDown = false;
	bool justChanged = (wasDown != isDown);
	keyState.m_wasJustReleased = true;

	keyState.m_isDown = isDown;
	keyState.m_justChanged = justChanged;
}

void InputSystem::SetCursorScreenPos(const IntVector2& pos)
{
	SetCursorPos(pos.x, pos.y);
}

IntVector2 InputSystem::GetCursorScreenPos() const
{
	POINT rawCursorPos;
	GetCursorPos(&rawCursorPos);
	return IntVector2(rawCursorPos.x, rawCursorPos.y);
}

IntVector2 InputSystem::GetScreenCenter() const
{
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	IntVector2 screenCenter;
	screenCenter.x = (desktopRect.right + desktopRect.left) / 2;
	screenCenter.y = (desktopRect.bottom + desktopRect.top) / 2;
	return screenCenter;
}

IntVector2 InputSystem::GetCursorWindowPos(HWND window) const
{
	POINT rawCursorPos;
	GetCursorPos(&rawCursorPos);
	ScreenToClient(window, &rawCursorPos);
	return IntVector2(rawCursorPos.x, rawCursorPos.y);
}

void InputSystem::ShowMouseCursor(bool isNowVisable)
{
	if (isNowVisable)
	{
		int currentShowCount = ShowCursor(TRUE);
		while (currentShowCount < 0)
		{
			currentShowCount = ShowCursor(TRUE);
		}
	}
	else
	{
		int currentShowCount = ShowCursor(FALSE);
		while (currentShowCount > -1)
		{
			currentShowCount = ShowCursor(FALSE);
		}
	}
}

bool InputSystem::isKeyDown(unsigned char keyIndex) const
{
	return m_keyStates[keyIndex].m_isDown;
}

bool InputSystem::wasKeyJustPressed(unsigned char keyIndex) const
{
	return isKeyDown(keyIndex) && m_keyStates[keyIndex].m_justChanged;
}

bool InputSystem::wasKeyJustReleased(unsigned char keyIndex) const
{
	return !isKeyDown(keyIndex) && m_keyStates[keyIndex].m_justChanged;
}

