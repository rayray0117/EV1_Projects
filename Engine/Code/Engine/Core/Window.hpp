#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Engine\Math\IntVector2.hpp"
#include <string>
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class Window;
class InputSystem;
//////////////////////////////////////////////////////////////////////////
typedef bool(*customMessageHandler_callback)(Window *wnd, UINT, WPARAM, LPARAM);
//////////////////////////////////////////////////////////////////////////
class Window
{
public:
	Window();
	~Window();

	// open() creates and shows the actual window.  Fails if this class already has a window open.
	bool open();
	HWND openHWND();
	void processMsgs();
	void close();

	void show(bool activate = true);

	bool isOpen(HWND hWnd);
	bool isOpen();
	inline bool is_closed() { return !isOpen(); }
	void setClientSize(unsigned int res_x, unsigned int res_y);
	void set_client_position( uint x, uint y, uint width, uint height );

	//setStyle();
	void center();

	IntVector2 getClientSize();
	IntVector2 getWindowSize();
	//void registerCustomMessageHandler()

	bool registerWindowClass();

	// Register a custom callback for the game to intercept windows messages
	// Passing in null unsets it.  
	void setCustomWndMsgHandler_callback(customMessageHandler_callback cb);

	HWND createTheWindow(int x, int y);

	Vector2 getCursorNormalizedPos(const IntVector2& rawPosition) const;
	Vector2 getCursorNormalizedPos(const InputSystem* inputSystem) const;
public:
	HWND hwnd;
	customMessageHandler_callback customMsgHandler;

	unsigned int clientWidth;
	unsigned int clientHeight;
	unsigned int windowWidth;
	unsigned int windowHeight;
	IntVector2 position;

	uint flags;

	std::string title;
	void set_title(char const *new_title);
};

static LRESULT CALLBACK windowMsgProc(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);