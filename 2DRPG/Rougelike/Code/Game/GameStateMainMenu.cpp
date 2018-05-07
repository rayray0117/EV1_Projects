#include "Game/GameStateMainMenu.hpp"
#include "Game/App.hpp"
#include "Game/Camera2D.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Rgba.hpp"
//////////////////////////////////////////////////////////////////////////
GameStateMainMenu::GameStateMainMenu()
{
	PlayMusic(g_menuMusic);
}

GameStateMainMenu::~GameStateMainMenu()
{

}
//////////////////////////////////////////////////////////////////////////
void GameStateMainMenu::Update(float /*deltaSeconds*/)
{
	HandleInput();
}

void GameStateMainMenu::Render() const
{
	drawMenuItems();
}
//////////////////////////////////////////////////////////////////////////
void GameStateMainMenu::HandleInput()
{
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_ESCAPE))
	{
		g_theApp->m_isQuitting = true;
	}
	else if (g_theGame->m_theWorld && (g_inputSystem->wasKeyJustPressed(KEYCODE_1) || wasInputJustPressed(INPUT_BACK)))
	{
		g_theGame->ChangeGameState(GAMESTATE_PLAY);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_2))
	{
		g_theGame->ChangeGameState(GAMESTATE_NEWGAMEMENU);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_3))
	{
		g_theApp->m_isQuitting = true; //#ToDo: make a global quit function!
	}
}

void GameStateMainMenu::drawMenuItems() const
{
	BitmapFont* font = g_renderer->CreateOrGetBitmapFont(DEFAULT_BITMAP_FONT_NAME);
	g_renderer->ClearScreen(Rgba::SKY_DAYLIGHT);
	g_renderer->SetOrthoProjection(IntVector2::ZERO, IntVector2(16, 9));
	g_renderer->DrawText2D(Vector2(1.9f, 6.9f), "Roguelike!", 1.25f, Rgba(32,32,32,64), 1.f, font); //Drop shadow :P
	g_renderer->DrawText2D(Vector2(2.f, 7.f), "Roguelike!", 1.25f, Rgba::WHITE, 1.f, font);

	Rgba resumeColor = Rgba::WHITE;
	if (!g_theGame->m_theWorld)
	{
		resumeColor = Rgba::GRAY_64;
	}
	g_renderer->DrawText2D(Vector2(4.f, 5.5f), "1) Resume",		1.f, resumeColor, .75f, font);
	g_renderer->DrawText2D(Vector2(4.f, 4.5f), "2) New Game",	1.f, Rgba::WHITE, .75f, font);
	g_renderer->DrawText2D(Vector2(4.f, 3.5f), "3) Quit",		1.f, Rgba::WHITE, .75f, font);

}
