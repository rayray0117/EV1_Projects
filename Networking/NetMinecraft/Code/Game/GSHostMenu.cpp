#include "Game/GSHostMenu.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
//////////////////////////////////////////////////////////////////////////
const uint MAX_OPTIONS = 4;
//////////////////////////////////////////////////////////////////////////
GSHostMenu::GSHostMenu()
{
	m_state = GAMESTATE_HOST_MENU;
}

GSHostMenu::~GSHostMenu()
{
	
}

void GSHostMenu::Update(float /*deltaSeconds*/)
{
	HandleInput();
}

void GSHostMenu::Render() const
{
	drawMenuItems();
}

void GSHostMenu::HandleInput()
{
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_ESCAPE))
	{
		g_theGame->ChangeGameState(GAMESTATE_MAINMENU);
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

void GSHostMenu::CallOption()
{
	switch (m_optionIndex)
	{
	case 0: CallSetSeedCmd();
		break;
	case 1: CallSetSaveCmd();
		break;
	case 2: g_theGame->ChangeGameState(GAMESTATE_HOST);
		break;
	case 3: g_theGame->ChangeGameState(GAMESTATE_MAINMENU);
	default:
		break;
	}
}

void GSHostMenu::drawMenuItems() const
{
	g_renderer->SetBlendFunc(RHIEnum::BLEND_SOURCE_ALPHA, RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA);
	g_renderer->EnableDepthTesting(false);
	g_renderer->EnableDepthWriting(false);
	BitmapFont* font = g_renderer->CreateOrGetBitmapFont(DEFAULT_FONT);
	g_renderer->ClearScreen(Rgba::SKY_DAYLIGHT);
	g_renderer->SetOrthoProjection(IntVector2::ZERO, IntVector2(16, 9));
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 7.f), "HOST MENU", 1.25f, Rgba::WHITE, 1.f, font);


	Rgba hostColor = m_optionIndex == 0 ? Rgba::YELLOW : Rgba::WHITE;
	Rgba joinColor = m_optionIndex == 1 ? Rgba::YELLOW : Rgba::WHITE;
	Rgba startColor = m_optionIndex == 2 ? Rgba::YELLOW : Rgba::WHITE;
	Rgba quitColor = m_optionIndex == 3 ? Rgba::YELLOW : Rgba::WHITE;
	std::string seedStr = Stringf("Seed: %i", g_Seed);
	//std::string saveStr = Stringf("Saving to: %s", g_savePath);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 5.5f), seedStr, 1.f, hostColor, .75f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 4.5f), "Saving to:", 1.f, joinColor, .75f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 3.75f), g_savePath, .5f, joinColor, .5f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 2.5f), "START", 1.f, startColor, .75f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 1.5f), "BACK", 1.f, quitColor, .75f, font);
}

void GSHostMenu::CallSetSeedCmd()
{
	g_theConsole->addMessage("Type in a number for the seed, then press enter!", 2.f, Rgba::GREEN);
	g_theConsole->m_currentBuffer = "set_seed ";
	g_consoleVisable = true;
}

void GSHostMenu::CallSetSaveCmd()
{

}
