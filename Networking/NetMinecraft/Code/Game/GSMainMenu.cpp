#include "Game/GSMainMenu.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Rgba.hpp"
//////////////////////////////////////////////////////////////////////////
const uint MAX_OPTIONS = 3;
//////////////////////////////////////////////////////////////////////////
GSMainMenu::GSMainMenu()
{
	m_state = GAMESTATE_MAINMENU;
}

GSMainMenu::~GSMainMenu()
{

}
//////////////////////////////////////////////////////////////////////////
void GSMainMenu::Update(float /*deltaSeconds*/)
{
	HandleInput();
}

void GSMainMenu::Render() const
{
	drawMenuItems();
}
//////////////////////////////////////////////////////////////////////////
void GSMainMenu::HandleInput()
{
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_ESCAPE))
	{
		g_theApp->m_isQuitting = true;
	}
	else //Trying out slightly different formating on my else if's to see if it feels more readable to me.
	if (wasInputJustPressed(INPUT_UP))
	{
		if (m_optionIndex == 0)
			return;

		--m_optionIndex;
	}
	else
	if (wasInputJustPressed(INPUT_DOWN))
	{
		if (m_optionIndex == MAX_OPTIONS-1)
			return;

		++m_optionIndex;
	}
	else
	if (wasInputJustPressed(INPUT_ACCEPT))
	{
		CallOption();
	}
}
//////////////////////////////////////////////////////////////////////////
void GSMainMenu::CallOption()
{
	switch (m_optionIndex)
	{
	case 0: g_theGame->ChangeGameState(GAMESTATE_HOST_MENU);
		break;
	case 1: g_theGame->ChangeGameState(GAMESTATE_JOIN);
		break;
	case 2: g_theApp->m_isQuitting = true;
		break;
	default:
		break;
	}
}

void GSMainMenu::drawMenuItems() const
{
	g_renderer->SetBlendFunc(RHIEnum::BLEND_SOURCE_ALPHA, RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA);
	g_renderer->EnableDepthTesting(false);
	g_renderer->EnableDepthWriting(false);
	BitmapFont* font = g_renderer->CreateOrGetBitmapFont(DEFAULT_FONT);
	g_renderer->ClearScreen(Rgba::SKY_DAYLIGHT);
	g_renderer->SetOrthoProjection(IntVector2::ZERO, IntVector2(16, 9));
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(7.9f, 6.9f), "Simple Net Miner", 1.f, Rgba(0, 0, 0, 64), 1.f, font); //Drop shadow :P
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 7.f), "Simple Net Miner", 1.f, Rgba::WHITE, 1.f, font);


	Rgba hostColor = m_optionIndex == 0 ? Rgba::YELLOW : Rgba::WHITE;
	Rgba joinColor = m_optionIndex == 1 ? Rgba::YELLOW : Rgba::WHITE;
	Rgba quitColor = m_optionIndex == 2 ? Rgba::YELLOW : Rgba::WHITE;
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 5.5f), "HOST", 1.f, hostColor, .75f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 4.5f), "JOIN", 1.f, joinColor, .75f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 3.5f), "QUIT", 1.f, quitColor, .75f, font);
}
