#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Renderer/RenderableString.hpp"
////////////////////////////////////////////////////////////////////////////
COMMAND(QUIT, "Quits the game.")
{
	g_theApp->m_isQuitting = true;
}
//////////////////////////////////////////////////////////////////////////
App::App()
	: m_isQuitting(false)
	, m_hasFocus(false)
{
	g_theConsole = Console::GetInstance();
	g_theConsole->m_haveConsolePrepareRendering = true;
	g_inputSystem = new InputSystem;
	g_theGame = new Game;
	g_renderer->timeData.system_frame_time = 0.f;
	g_renderer->timeData.system_time = 0.f;
}

App::~App()
{
	SAFE_DELETE(g_theGame);

	SAFE_DELETE(g_renderer);

	SAFE_DELETE(g_inputSystem);
}

void App::setIsQuitting(bool isNowQuitting)
{
	m_isQuitting = isNowQuitting;
}

void App::BeginFrame()
{
	if (g_inputSystem)
		g_inputSystem->BeginFrame();

	
}

void App::RunFrame()
{
	static double timeOfLastRunFrame = GetCurrentTimeSeconds(); // Only happens once

	BeginFrame();

	double timeNow = GetCurrentTimeSeconds();
	double deltaSeconds = timeNow - timeOfLastRunFrame;
	timeOfLastRunFrame = timeNow;

	g_renderer->timeData.system_frame_time = (float)deltaSeconds;
	g_renderer->timeData.system_time += (float)deltaSeconds;
	Update(static_cast<float>(deltaSeconds));
	Render();
	EndFrame();
}

void App::EndFrame()
{
	if (g_inputSystem)
		g_inputSystem->EndFrame();
	if(g_consoleVisable)
		g_theConsole->Render(g_renderer);
}

void App::OnKeyPressed(unsigned char keyCode)
{
	g_inputSystem->OnKeyPressed(keyCode);
	
	if (keyCode == KEYCODE_ESCAPE)
	{
		m_isQuitting = true;
	}
	if (keyCode == KEYCODE_TILDE)
	{
		g_consoleVisable = true;
		g_theConsole->addMessage(RenderableString("Console Enabled", 1.f, Rgba::GREEN));
	}
}

void App::OnKeyReleased(unsigned char keyCode)
{
	g_inputSystem->OnKeyReleased(keyCode);
}

void App::OnGainedFocus()
{
	DebuggerPrintf("Gained focus!\n");
	g_inputSystem->ShowMouseCursor(false);
	m_hasFocus = true;
}

void App::OnLostFocus()
{
	DebuggerPrintf("lost focus!\n");
	g_inputSystem->ShowMouseCursor(true);
	m_hasFocus = false;
}

void App::Update(float deltaSeconds)
{
	IntVector2 screenCenter = g_inputSystem->GetScreenCenter();
	IntVector2 cursorPos = g_inputSystem->GetCursorScreenPos();

	if (g_theGame)
	{
		if(deltaSeconds > 0.1f)
			g_theGame->Update(0.1f);
		else
			g_theGame->Update(deltaSeconds);
	}
}

//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	if (g_theGame)
		g_theGame->Render();
	if (g_inputSystem)
		g_inputSystem->Render();
}