#include "Engine/Core/Window.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
//////////////////////////////////////////////////////////////////////////
static char const*const WNDCLASS_GAME_NAME = "ITWGameWindow";
//////////////////////////////////////////////////////////////////////////
static HINSTANCE GetCurrentHINSTANCE()
{
	return ::GetModuleHandle(NULL);
}
static DWORD InternalGetStyle(bool visible = true)
{
	DWORD ret = 0;
	ret |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER);

	// if visible
	if (visible) {
		ret |= WS_VISIBLE;
	}

	return ret;
}
static RECT InternalGetRect(int x, int y, int width, int height, uint style)
{
	RECT out_rect;
	out_rect.left = 0;
	out_rect.top = 0;
	out_rect.right = width;
	out_rect.bottom = height;
	::AdjustWindowRect(&out_rect, style, FALSE);

	out_rect.left += x;
	out_rect.top += y;
	out_rect.right += x;
	out_rect.bottom += y;

	return out_rect;
}
static RECT WindowGetRect(Window *wnd)
{
	DWORD style = InternalGetStyle();

	RECT win_rect;

	win_rect = InternalGetRect(wnd->position.x, wnd->position.y,
		wnd->clientWidth, wnd->clientHeight,
		style);

	wnd->windowWidth = wnd->clientWidth;
	wnd->windowHeight = wnd->clientHeight;

	return win_rect;
}
#define SET_BIT_FLAG(v, f)             (v) = (v) | (f)
#define UNSET_BIT_FLAG(v, f)           (v) = (v) & ~(f)
static bool WindowUpdateStyleAndPosition(Window *wnd)
{
	HWND hwnd = wnd->hwnd;
	if (hwnd != NULL) {
		DWORD style = InternalGetStyle();

		uint WNDFLAG_LOCKED = ((1U << (3)));

		// do the window stuff
		SET_BIT_FLAG(wnd->flags, WNDFLAG_LOCKED);
		SetWindowLongPtr(hwnd, GWL_STYLE, style);
		UNSET_BIT_FLAG(wnd->flags, WNDFLAG_LOCKED);

		RECT win_rect = WindowGetRect(wnd);
		if (FALSE == ::SetWindowPos(hwnd, NULL,
			win_rect.left, win_rect.top,
			win_rect.right - win_rect.left,
			win_rect.bottom - win_rect.top,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED)) {

			return false;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
Window::Window()
	: position(0,0)
	, clientWidth(640)
	, clientHeight(480)
	, title("WindowTitleBar")
	, customMsgHandler(nullptr)
{
	registerWindowClass();
}

Window::~Window()
{
	close();
}
//////////////////////////////////////////////////////////////////////////
bool Window::open()
{
	if (isOpen()) {
		return false;
	}

	hwnd = createTheWindow(position.x, position.y);
	return true;
}

HWND Window::openHWND()
{
	return createTheWindow(position.x, position.y);
}

void Window::processMsgs()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, hwnd, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}

void Window::close()
{
	if (!isOpen()) {
		return;
	}

	// on_closed.trigger(this);
	::DestroyWindow(hwnd);
	hwnd = nullptr;

	// Removes all the flags
	flags = 0U;
}

void Window::show(bool activate /*= true*/)
{
	if (isOpen() /*&& !is_visible()*/) {
		int const command = activate ? SW_SHOW : SW_SHOWNOACTIVATE;
		::ShowWindow(hwnd, command);
	}
}

bool Window::isOpen(HWND hWnd)
{
	return !!::IsWindow(hWnd);
}

bool Window::isOpen()
{
	return !!::IsWindow(hwnd);
}

void Window::setClientSize(unsigned int res_x, unsigned int res_y)
{
	// #ToDo: accutal
	clientWidth = res_x;
	clientHeight = res_y;
}

void Window::set_client_position(uint x, uint y, uint width, uint height)
{
	clientWidth = (int)width;
	clientHeight = (int)height;
	position.x = (int)x;
	position.y = (int)y;

	if (isOpen()) {
		WindowUpdateStyleAndPosition(this);
	}
}

void Window::center()
{

}

IntVector2 Window::getClientSize()
{
	return IntVector2(clientWidth, clientHeight);
}

IntVector2 Window::getWindowSize()
{
	return IntVector2(windowWidth, windowHeight);
}

bool Window::registerWindowClass()
{
	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(windowMsgProc); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetCurrentHINSTANCE();


	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;

	// Cursor to use (can be set later if needed)
	windowClassDescription.hCursor = ::LoadCursorA(NULL, IDC_ARROW);
	windowClassDescription.hbrBackground = (HBRUSH)0;
	windowClassDescription.lpszMenuName = NULL;

	windowClassDescription.lpszClassName = TEXT("ITWGameWindow");
	ATOM result = RegisterClassEx(&windowClassDescription);
	if (NULL == result) {
		return false;
	}

	return true;
}

void Window::setCustomWndMsgHandler_callback(customMessageHandler_callback cb)
{
	customMsgHandler = cb;
}

HWND Window::createTheWindow(int x, int y)
{
	const DWORD windowStyleFlags = InternalGetStyle(false);
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT windowRect = WindowGetRect(this);


	x += windowRect.left;
	y += windowRect.top;
	int w = windowRect.right - windowRect.left;
	int h = windowRect.bottom - windowRect.top;

	HWND hWnd = CreateWindowExA(windowStyleExFlags, // Extended style (not used)
		WNDCLASS_GAME_NAME,                       // Windows Class to use
		title.c_str(),                                    // title      
		windowStyleFlags,                                    // Style to start this with
		x, y,                                     // x, y of actual window (not client rect)
		w, h,                                     // width and height of actual window
		NULL,                                     // my parent (not used in this example)
		NULL,                                     // my menu (not used in this example)
		GetCurrentHINSTANCE(),                    // hinstance this window belongs too
		this);                                // user data associated with this window (good place to store a pointer to a window object if you class'ify this)

	return hWnd;
}

Vector2 Window::getCursorNormalizedPos(const IntVector2& rawPosition) const
{
	Vector2 normalizedPos;
	normalizedPos.x = rangeMapFloat((float)rawPosition.x, 0.f, (float)clientWidth, 0.f, 1.f);
	normalizedPos.y = rangeMapFloat((float)rawPosition.y, 0.f, (float)clientHeight, 0.f, 1.f);
	return normalizedPos;
}

Vector2 Window::getCursorNormalizedPos(const InputSystem* inputSystem) const
{
	IntVector2 rawPosition = inputSystem->GetCursorWindowPos(hwnd);

	
	return getCursorNormalizedPos(rawPosition);
}

void Window::set_title(char const *new_title)
{
	if (title != new_title) {
		title = new_title;
		if (hwnd != NULL) {
			::SetWindowTextA(hwnd, new_title);
		}
	}
}

static LRESULT CALLBACK windowMsgProc(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
 	Window *wnd = (Window*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);
 	if ((wnd != nullptr) && (wnd->customMsgHandler != nullptr))
 	{
 		bool stopProcess = wnd->customMsgHandler(wnd, wmMessageCode, wParam, lParam);
 		if (stopProcess)
 		{
 			return 0;
 		}
 	}
 
 	switch (wmMessageCode)
 	{
 		case WM_CREATE: {
 			// Window creation happened.
 
 			// Grabbing the user data passed with WindowCreate
 
 			CREATESTRUCT *cp = (CREATESTRUCT*)lParam;
 			wnd = (Window*)cp->lpCreateParams;
 
 			wnd->hwnd = windowHandle;
 			SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)wnd);
 
 		} break;
 	}

	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}