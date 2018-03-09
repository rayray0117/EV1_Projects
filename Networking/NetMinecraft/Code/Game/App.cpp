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
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
#include "BlockDefinition.hpp"
////////////////////////////////////////////////////////////////////////////
COMMAND(quit, "Quits the game.")
{
	g_theApp->m_isQuitting = true;
}

JobConsumer* g_MainThreadConsumer = nullptr;
//////////////////////////////////////////////////////////////////////////
App::App()
	: m_isQuitting(false)
	, m_hasFocus(false)
{
	g_theConsole = Console::GetInstance();
	g_theConsole->m_haveConsolePrepareRendering = true;
	g_inputSystem = new InputSystem;
	g_audioSystem = new AudioSystem;
	g_theGame = new Game;
	g_renderer->timeData.system_frame_time = 0.f;
	g_renderer->timeData.system_time = 0.f;
	g_MainThreadConsumer = new JobConsumer();
	g_MainThreadConsumer->AddCategory(JOB_MAIN);

	BlockDefinition::initializeBlockDefs();
	g_theGame->ChangeGameState(GAMESTATE_MAINMENU);
}

App::~App()
{
	SAFE_DELETE(g_MainThreadConsumer);
	SAFE_DELETE(g_theGame);
	SAFE_DELETE(g_audioSystem);
	SAFE_DELETE(g_renderer);

	SAFE_DELETE(g_inputSystem);
}

void App::setIsQuitting(bool isNowQuitting)
{
	m_isQuitting = isNowQuitting;
}

void App::BeginFrame()
{
	MemTrac::ProfileMemoryFrameTick();

	if (g_inputSystem)
		g_inputSystem->BeginFrame();
}

void App::RunFrame()
{
	PROFILE_SCOPE_FUNCTION();
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

	g_MainThreadConsumer->ConsumeJob();// Consume one cus I don't have consume for ms yet and don't want to drop frames waiting for long jobs.
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
	if (g_theConsole->m_anim)
	{
		if (deltaSeconds > 0.1f)
			g_theConsole->m_anim->Update(0.1f);
		else
		{
			g_theConsole->m_anim->Update(deltaSeconds);
		}

	}
	if (g_audioSystem)
		g_audioSystem->Update();
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
