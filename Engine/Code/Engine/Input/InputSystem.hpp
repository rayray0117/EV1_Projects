#pragma once
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/IntVector2.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//////////////////////////////////////////////////////////////////////////
class IntVector2;
//////////////////////////////////////////////////////////////////////////
enum KeyCode
{
	KEYCODE_LMB			= 0x01,
	KEYCODE_RMB			= 0x02,
	KEYCODE_MMB			= 0x04,
	KEYCODE_BACKSPACE	= 0x08,
	KEYCODE_TAB			= 0x09,
	KEYCODE_ENTER		= 0x0D,
	KEYCODE_SHIFT		= 0x10,
	KEYCODE_CONTROL		= 0x11,
	KEYCODE_ALT			= 0x12,
	KEYCODE_PAUSE		= 0x13,
	KEYCODE_CAPSLOCK	= 0x14,
	KEYCODE_CAPITAL		= KEYCODE_CAPSLOCK,
	KEYCODE_ESCAPE		= 0x1B,
	KEYCODE_SPACE		= 0x20,
	KEYCODE_PAGEUP		= 0x21,
	KEYCODE_PRIOR		= KEYCODE_PAGEUP,
	KEYCODE_PAGEDOWN	= 0x22,
	KEYCODE_NEXT		= KEYCODE_PAGEDOWN,
	KEYCODE_END			= 0x23,
	KEYCODE_HOME		= 0x24,
	KEYCODE_LEFT		= 0x25,
	KEYCODE_UP			= 0x26,
	KEYCODE_RIGHT		= 0x27,
	KEYCODE_DOWN		= 0x28,
	KEYCODE_PRINTSCREEN = 0x2C,
	KEYCODE_PRTSCRN		= KEYCODE_PRINTSCREEN,
	KEYCODE_PRINTSCRN	= KEYCODE_PRINTSCREEN,
	KEYCODE_PRINT		= KEYCODE_PRINTSCREEN,
	KEYCODE_INSERT		= 0x2D,
	KEYCODE_DELETE		= 0x2E,
	//
	KEYCODE_0			= 0x30,
	KEYCODE_1			= 0x31,
	KEYCODE_2			= 0x32,
	KEYCODE_3			= 0x33,
	KEYCODE_4			= 0x34,
	KEYCODE_5			= 0x35,
	KEYCODE_6			= 0x36,
	KEYCODE_7			= 0x37,
	KEYCODE_8			= 0x38,
	KEYCODE_9			= 0x39,
	KEYCODE_A			= 0x41,
	KEYCODE_B			= 0x42,
	KEYCODE_C			= 0x43,
	KEYCODE_D			= 0x44,
	KEYCODE_E			= 0x45,
	KEYCODE_F			= 0x46,
	KEYCODE_G			= 0x47,
	KEYCODE_H			= 0x48,
	KEYCODE_I			= 0x49,
	KEYCODE_J			= 0x4A,
	KEYCODE_K			= 0x4B,
	KEYCODE_L			= 0x4C,
	KEYCODE_M			= 0x4D,
	KEYCODE_N			= 0x4E,
	KEYCODE_O			= 0x4F,
	KEYCODE_P			= 0x50,
	KEYCODE_Q			= 0x51,
	KEYCODE_R			= 0x52,
	KEYCODE_S			= 0x53,
	KEYCODE_T			= 0x54,
	KEYCODE_U			= 0x55,
	KEYCODE_V			= 0x56,
	KEYCODE_W			= 0x57,
	KEYCODE_X			= 0x58,
	KEYCODE_Y			= 0x59,
	KEYCODE_Z			= 0x5A,
	//
	KEYCODE_LEFTWINDOWS = 0x5B,
	KEYCODE_LWINDOWS	= KEYCODE_LEFTWINDOWS,
	KEYCODE_LWIN		= KEYCODE_LEFTWINDOWS,
	KEYCODE_RIGHTWINDOWS = 0x5C,
	KEYCODE_RWINDOWS	= KEYCODE_RIGHTWINDOWS,
	KEYCODE_RWIN		= KEYCODE_RIGHTWINDOWS,
	KEYCODE_NUMPAD0		= 0x60,
	KEYCODE_NUMPAD1		= 0x61,
	KEYCODE_NUMPAD2		= 0x62,
	KEYCODE_NUMPAD3		= 0x63,
	KEYCODE_NUMPAD4		= 0x64,
	KEYCODE_NUMPAD5		= 0x65,
	KEYCODE_NUMPAD6		= 0x66,
	KEYCODE_NUMPAD7		= 0x67,
	KEYCODE_NUMPAD8		= 0x68,
	KEYCODE_NUMPAD9		= 0x69,
	KEYCODE_MULTIPLY	= 0x6A,
	KEYCODE_ADD			= 0x6B,
	KEYCODE_NUMPADENTER = 0x6c,
	KEYCODE_SEPARATOR	= KEYCODE_NUMPADENTER,
	KEYCODE_SUBTRACT	= 0x6D,
	KEYCODE_DECIMAL		= 0x6E,
	KEYCODE_DIVIDE		= 0x6F,
	KEYCODE_F1			= 0x70,
	KEYCODE_F2			= 0x71,
	KEYCODE_F3			= 0x72,
	KEYCODE_F4			= 0x73,
	KEYCODE_F5			= 0x74,
	KEYCODE_F6			= 0x75,
	KEYCODE_F7			= 0x76,
	KEYCODE_F8			= 0x77,
	KEYCODE_F9			= 0x78,
	KEYCODE_F10			= 0x79,
	KEYCODE_F11			= 0x7A,
	KEYCODE_F12			= 0x7B,
	KEYCODE_F14			= 0x7C,
	KEYCODE_F15			= 0x7D,
	KEYCODE_F16			= 0x7E,
	KEYCODE_F17			= 0x7F,
	KEYCODE_F18			= 0x80,
	KEYCODE_F19			= 0x81,
	KEYCODE_F20			= 0x82,
	KEYCODE_F21			= 0x83,
	KEYCODE_F22			= 0x84,
	KEYCODE_F23			= 0x85,
	KEYCODE_F24			= 0x86,
	KEYCODE_F25			= 0x87,
	KEYCODE_NUMLOCK		= 0x90,
	KEYCODE_SCROLLLOCK	= 0x91,
	KEYCODE_NUMPADEQUALS = 0x92,
	KEYCODE_SEMICOLON	= 0xBA,
	KEYCODE_EQUALS		= 0xBB,
	KEYCODE_COMMA		= 0xBC,
	KEYCODE_MINUS		= 0xBD,
	KEYCODE_PERIOD		= 0xBE,
	KEYCODE_FORWARDSLASH = 0xBF,
	KEYCODE_FSLASH		= KEYCODE_FORWARDSLASH,
	KEYCODE_BACKQUOTE	= 0xC0,
	KEYCODE_TILDE		= KEYCODE_BACKQUOTE,
	KEYCODE_LEFTBRACKET = 0xDB,
	KEYCODE_LEFTSQUAREBRACKET = KEYCODE_LEFTBRACKET,
	KEYCODE_LBRACKET	= KEYCODE_LEFTBRACKET,
	KEYCODE_LSQBRACKET	= KEYCODE_LEFTBRACKET,
	KEYCODE_BACKSLASH	= 0xDC,
	KEYCODE_BSLASH		= KEYCODE_BACKSLASH,
	KEYCODE_RIGHTBRACKET = 0xDD,
	KEYCODE_RIGHTSQUAREBRACKET = KEYCODE_RIGHTBRACKET,
	KEYCODE_RBRACKET	= KEYCODE_RIGHTBRACKET,
	KEYCODE_RSQBRACKET	= KEYCODE_RIGHTBRACKET,
	KEYCODE_APOSTROPHE	= 0xDE,
	KEYCODE_SINGLEQUOTE = KEYCODE_APOSTROPHE,
	KEYCODE_UNKNOWN		= 0xFF,
	KEYCODE_LSHIFT		= 0xA0,
	KEYCODE_RSHIFT		= 0xA1,
	KEYCODE_LCONTROL	= 0xA2,
	KEYCODE_RCONTROL	= 0xA3,
	NUM_KEYBOARD_KEYS	= 256
};

struct KeyState
{
	bool m_isDown;
	bool m_justChanged;
	bool m_wasJustPressed;
	bool m_wasJustReleased;
};

//////////////////////////////////////////////////////////////////////////
class InputSystem 
{
public:
	InputSystem();
	~InputSystem();

	void BeginFrame();
	void EndFrame();
	void Render() const;

	//Updates (from Main_Win32)
	void OnKeyPressed(unsigned char keyIndex);
	void OnKeyReleased(unsigned char keyIndex);
	void SetCursorScreenPos(const IntVector2& pos);
	IntVector2 GetCursorScreenPos() const;
	IntVector2 GetScreenCenter() const;
	IntVector2 GetCursorWindowPos(HWND window) const;
	void ShowMouseCursor(bool isNowVisable);

	// Queries
	bool isKeyDown(unsigned char keyIndex) const;
	bool wasKeyJustPressed(unsigned char keyIndex) const;
	bool wasKeyJustReleased(unsigned char keyIndex) const;
	

	const XboxController& GetControllerState(int controllerID) const { return m_xboxControllers[controllerID]; }

private:
	KeyState m_keyStates[NUM_KEYBOARD_KEYS];
	XboxController m_xboxControllers[MAX_XBOX_CONTROLLERS];
};