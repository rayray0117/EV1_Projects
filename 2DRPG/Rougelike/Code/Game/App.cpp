#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Renderer/RenderableString.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
////////////////////////////////////////////////////////////////////////////
COMMAND(quit, "Quits the game.")
{
	g_theApp->m_isQuitting = true;
}
//////////////////////////////////////////////////////////////////////////
App::App()
	: m_isQuitting(false)
	, m_hasFocus(false)
{
	g_theConsole = CONSOLE->GetInstance();
	g_inputSystem = new InputSystem;
	g_audioSystem = new AudioSystem;
	g_theGame = new Game;

	g_theGame->ChangeGameState(GAMESTATE_MAINMENU);
}

App::~App()
{
	SAFE_DELETE(g_theGame);
	SAFE_DELETE(g_renderer);
	SAFE_DELETE(g_audioSystem);
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

	Update(static_cast<float>(deltaSeconds));
	Render();
	EndFrame();
}

void App::EndFrame()
{
	if (g_inputSystem)
		g_inputSystem->EndFrame();
}

void App::OnKeyPressed(unsigned char keyCode)
{
	g_inputSystem->OnKeyPressed(keyCode);

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
	//g_inputSystem->ShowMouseCursor(false);
	m_hasFocus = true;
}

void App::OnLostFocus()
{
	//g_inputSystem->ShowMouseCursor(true);
	m_hasFocus = false;
}

void App::Update(float deltaSeconds)
{
	if (g_theConsole->m_anim)
	{
		if (deltaSeconds > 0.1f)
			g_theConsole->m_anim->Update(0.1f);
		else
		{
			g_theConsole->m_anim->Update(deltaSeconds);
		}
		
	}
	if (g_theGame)
	{
		if (deltaSeconds > 0.1f)
			g_theGame->Update(0.1f);
		else
			g_theGame->Update(deltaSeconds);
	}
}

//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	g_renderer->ClearScreen(Rgba::BLACK);


	if (g_theGame)
		g_theGame->Render();
	if (g_inputSystem)
		g_inputSystem->Render();
	if (g_consoleVisable)
		g_theConsole->Render(g_renderer);
}
