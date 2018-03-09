#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>					// #ToDo: Remove all references to OpenGL
#include <cassert>
#include <crtdbg.h>
#include <stdlib.h>
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/RHI/RHI.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/JobSystem.hpp"
//-----------------------------------------------------------------------------------------------
HWND g_hWnd = nullptr;
//-----------------------------------------------------------------------------------------------
bool WindowsMessageHandlingProcedure(Window* wnd, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	UNUSED(lParam);
	UNUSED(wnd);

	unsigned char asKey = (unsigned char)wParam;
	switch (wmMessageCode)
	{

	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		if (g_theApp)
			g_theApp->setIsQuitting(true);
		return 0;


	case WM_KEYDOWN:
		if (g_theApp)
		{
			if (g_consoleVisable)
			{
				switch (asKey)
				{
				case KEYCODE_ENTER:
				case KEYCODE_TILDE:
				case KEYCODE_UP:
				case KEYCODE_DOWN:
				case KEYCODE_LEFT:
				case KEYCODE_RIGHT:
				case KEYCODE_ESCAPE:
				case KEYCODE_BACKSPACE:
				case KEYCODE_NUMPADENTER:
				case KEYCODE_TAB:
					g_theConsole->OnKeyCodePressed(asKey);
					break;
				default:
					break;
				}
			}
			else
			{
				g_theApp->OnKeyPressed(asKey);
			}

		}
		break;

	case  WM_CHAR:
		if (g_theApp)
		{
			if (g_consoleVisable)
			{
				switch (asKey)
				{
				case '`':
				case KEYCODE_BACKSPACE:
				case KEYCODE_ESCAPE:
				case KEYCODE_ENTER: //enter key aka carriage return
				case KEYCODE_TAB:
					break;
				default:
					g_theConsole->OnKeyPressed(asKey);
					break;
				}
			}
		}
		break;


	case WM_KEYUP:
		if (g_theApp)
			g_theApp->OnKeyReleased(asKey);
		break;

	case  WM_LBUTTONDOWN:
		if (g_theApp)
			g_theApp->OnKeyPressed(KEYCODE_LMB);
		break;

	case  WM_LBUTTONUP:
		if (g_theApp)
			g_theApp->OnKeyReleased(KEYCODE_LMB);
		break;

	case  WM_RBUTTONDOWN:
		if (g_theApp)
			g_theApp->OnKeyPressed(KEYCODE_RMB);
		break;

	case  WM_RBUTTONUP:
		if (g_theApp)
			g_theApp->OnKeyReleased(KEYCODE_RMB);
		break;

	case WM_MOUSEHWHEEL:
		if (g_theApp)
		{

		}

	case WM_SETFOCUS:
		if (g_theApp)
			g_theApp->OnGainedFocus();
		break;

	case WM_KILLFOCUS:
		if (g_theApp)
			g_theApp->OnLostFocus();
		break;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
// Processes all windows messages (WM_xxx) for this app that have queued up since last frame.
//
void RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}

//-----------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------
void Initialize(HINSTANCE applicationInstanceHandle)
{
	UNUSED(applicationInstanceHandle);
	//CallstackSystemInit();
	JobSystemStartup(NUM_JOB_TYPES);
	//Log::StartUp("Data/LogTest.log");
	Profiler::GetInstance()->StartUp();


	SetProcessDPIAware();
	g_gameConfig = new Config();
	setupGameConfig("Data/Config/Settings.config");

	g_renderer = new SimpleRenderer();
	g_mainEngineRenderer = g_renderer;
	g_renderer->setup(g_windowWidth, g_windowHeight);

	g_myWindow = g_renderer->output->m_window;
	g_hWnd = g_myWindow->hwnd;

	g_theApp = new App();
	g_myWindow->setCustomWndMsgHandler_callback(WindowsMessageHandlingProcedure);
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	delete g_theApp;
	g_theApp = nullptr;

	SAFE_DELETE(g_gameConfig);


	Profiler::GetInstance()->Shutdown();
	//MemTrac::LogLiveAllocations();
	//Log::Shutdown();
	JobSystemShutdown();
	//CallstackSystemDeinit();
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
	UNUSED(commandLineString);
	Initialize(applicationInstanceHandle);

	while (!g_theApp->m_isQuitting && g_myWindow->isOpen(g_hWnd))
	{
		static double timeOfLastRunFrame = GetCurrentTimeSeconds(); // Only happens once
		PROFILE_SCOPE_FUNCTION();

		double timeNow = GetCurrentTimeSeconds();
		timeOfLastRunFrame = timeNow;

		g_myWindow->processMsgs();
		g_theApp->RunFrame();
		g_renderer->present();

		double elapsedTime = GetCurrentTimeSeconds() - timeOfLastRunFrame;
		if (elapsedTime < g_targetTime)
			Sleep(1);

		while (elapsedTime < g_targetTime)
		{
			elapsedTime = GetCurrentTimeSeconds() - timeOfLastRunFrame;
		}


	}

	Shutdown();
	return 0;
}