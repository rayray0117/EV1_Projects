#include "Game/GameStateNewGameMenu.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/Camera2D.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
//////////////////////////////////////////////////////////////////////////
std::vector<std::string> g_adventureNames; //Makes it easier to display and choose a particular adventure
constexpr int menu_width = 16*3/2;
constexpr int menu_height = 9*3/2;
constexpr int menu_half_width = menu_width / 2;
constexpr int menu_half_height = menu_height / 2;
//constexpr int menu_threequarter_width = menu_width * 3 / 4;
constexpr int menu_threequarter_height = menu_height * 3 / 4;
const Vector2 tileCenterPos			= Vector2((float)menu_half_width, (float)menu_threequarter_height);
const Vector2 tileShadowCenterPos	= Vector2((float)menu_half_width, (float)menu_threequarter_height) - Vector2(.1f, .1f);
const Vector2 text1CenterPos		= Vector2((float)menu_half_width, (float)menu_half_height);
const Vector2 text2CenterPos		= Vector2((float)menu_half_width, (float)menu_half_height) - Vector2(0.f, 1.f);
const Vector2 text3CenterPos		= Vector2((float)menu_half_width, 1.f);
//////////////////////////////////////////////////////////////////////////
GameStateNewGameMenu::GameStateNewGameMenu()
{
	//Main Menu should have already set music so don't need to set it here.
	SetupAdventureNames();

}
//////////////////////////////////////////////////////////////////////////
void GameStateNewGameMenu::Update(float /*deltaSeconds*/)
{
	HandleInput();
}

void GameStateNewGameMenu::Render() const
{
	drawMenuItems();
}
////////////////////////////////////////////////////////////////////////// Private //////////////////////////////////////////////////////////////////////////
void GameStateNewGameMenu::HandleInput()
{
	if (wasInputJustPressed(INPUT_BACK))
	{
		g_theGame->ChangeGameState(GAMESTATE_MAINMENU);
	}
	else if (g_adventureNames.size() > 0 && wasInputJustPressed(INPUT_ACCEPT))
	{
		LoadCurrentOption();
	}
	else if (wasInputJustPressed(INPUT_LEFT) || wasInputJustPressed(INPUT_DOWN))
	{
		DecrementOption();
	}
	else if (wasInputJustPressed(INPUT_RIGHT) || wasInputJustPressed(INPUT_UP))
	{
		IncrementOption();
	}
}

void GameStateNewGameMenu::drawMenuItems() const
{
	BitmapFont* font = g_renderer->CreateOrGetBitmapFont(DEFAULT_BITMAP_FONT_NAME);
	g_renderer->ClearScreen(Rgba::SKY_DAYLIGHT);
	g_renderer->SetOrthoProjection(IntVector2::ZERO, IntVector2(menu_width, menu_height));
	g_renderer->drawText2DWithBitmapFont_Centered(tileShadowCenterPos, "Pick an Adventure!", 1.25f, Rgba(32, 32, 32, 64), 1.f, font); //Drop shadow :P
	g_renderer->drawText2DWithBitmapFont_Centered(tileCenterPos, "Pick an Adventure!", 1.25f, Rgba::WHITE, 1.f, font);

	//Set up text for currently loaded adventure
	Rgba resumeColor = Rgba::WHITE;
	std::string currentLoadedAdventureTxt = "Current: " + g_theGame->m_currentAdventure;
	if (g_theGame->m_currentAdventure == "None")
	{
		resumeColor = Rgba::GRAY_64;
	}

	//Set up test for adventure to load
	std::string adventureToLoadTxt = g_adventureNames.empty() ? "No adventures available!" : g_adventureNames[m_currentAdventureOption];
	Rgba toLoadColor = g_adventureNames.empty() ? Rgba::RED : Rgba::WHITE;

	static const float loadedWidth = font->getTextWidth("Current:", 1.f, .75f);
	static const float loadWidth = font->getTextWidth("To Load:", 1.f, .75f);

	g_renderer->DrawText2D(text1CenterPos - Vector2(loadedWidth, 0.f), currentLoadedAdventureTxt, 1.f, resumeColor, .75f, font);
	g_renderer->DrawText2D(text2CenterPos - Vector2(loadWidth, 0.f), "To Load: " + adventureToLoadTxt, 1.f, toLoadColor, .75f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(text3CenterPos, "< Previous |   Next   >", 1.f, toLoadColor, .75f, font);
}

void GameStateNewGameMenu::DecrementOption()
{
	if (m_currentAdventureOption == 0)
		m_currentAdventureOption = g_adventureNames.size() - 1;
	else
		--m_currentAdventureOption;
}

void GameStateNewGameMenu::IncrementOption()
{
	if (m_currentAdventureOption == g_adventureNames.size() - 1)
		m_currentAdventureOption = 0;
	else
		++m_currentAdventureOption;
}

void GameStateNewGameMenu::LoadCurrentOption()
{
	g_theGame->LoadNewAdventure(g_adventureNames[m_currentAdventureOption]);
}

void GameStateNewGameMenu::SetupAdventureNames()
{
	m_currentAdventureOption = 0;
	g_adventureNames.clear();
	for (auto current : AdventureDefinition::s_registry)
	{
		g_adventureNames.push_back(current.first);
	}
}
