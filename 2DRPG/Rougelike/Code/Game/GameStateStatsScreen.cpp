#include "Game/GameStateStatsScreen.hpp"
#include "Game/World.hpp"
#include "Game/Camera2D.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Game/Controller.hpp"
#include "Game/Pawn.hpp"
#include "Engine/Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
GameStateStatsScreen::GameStateStatsScreen()
{
	mo_world = g_theGame->m_theWorld;
	mo_camera = g_theGame->m_theCamera;
}

GameStateStatsScreen::~GameStateStatsScreen()
{

}
//////////////////////////////////////////////////////////////////////////
void GameStateStatsScreen::Update(float /*deltaSeconds*/)
{
	if (wasInputJustPressed(INPUT_STATSCREEN) || wasInputJustPressed(INPUT_START))
	{
		g_theGame->ChangeGameState(GAMESTATE_PLAY);
		return;
	}
}

void GameStateStatsScreen::Render() const
{
	//Render camera's view.
	mo_camera->Render();

	//Begin rendering real game stuff
	mo_world->Render();

	//Draw translucent background
	g_renderer->SetOrthoProjection(IntVector2::ZERO, IntVector2::ONE);
	g_renderer->drawAABB2(AABB2::ZERO_TO_ONE, Rgba(0, 0, 0, 64));

	Render_StatsScreen();
}

void GameStateStatsScreen::Render_StatsScreen() const
{
	g_renderer->SetOrthoProjection(IntVector2::ZERO, IntVector2(16,9));

	Controller* playerController = Controller::s_controllers[0];
	if (playerController->mo_controlledPawn == nullptr)
		return;

	Stats playerModStats = playerController->mo_controlledPawn->getCombinedStats();
	Stats playerBaseStats = playerController->mo_controlledPawn->m_baseStats;

	std::string heathTxt = Stringf("Health:   %i | %i", playerBaseStats.m_stats[STAT_HEALTH], playerModStats.m_stats[STAT_HEALTH]);
	std::string stamTxt  = Stringf("Stamina:  %i | %i", playerBaseStats.m_stats[STAT_STAMINA], playerModStats.m_stats[STAT_STAMINA]);
	std::string speedTxt = Stringf("Speed:    %i | %i", playerBaseStats.m_stats[STAT_SPEED], playerModStats.m_stats[STAT_SPEED]);
	std::string moveTxt  = Stringf("Movement: %i | %i", playerBaseStats.m_stats[STAT_MOVEMENT], playerModStats.m_stats[STAT_MOVEMENT]);
	std::string attTxt   = Stringf("Attack:   %i | %i", playerBaseStats.m_stats[STAT_ATTACK], playerModStats.m_stats[STAT_ATTACK]);
	std::string defTxt   = Stringf("Defense:  %i | %i", playerBaseStats.m_stats[STAT_DEFENSE], playerModStats.m_stats[STAT_DEFENSE]);
	std::string magTxt   = Stringf("Magic:    %i | %i", playerBaseStats.m_stats[STAT_MAGIC], playerModStats.m_stats[STAT_MAGIC]);


	BitmapFont* font = g_renderer->CreateOrGetBitmapFont(g_bitmapFont);
	g_renderer->DrawText2D(Vector2(3.f, 7.5f), heathTxt, 1.f, Rgba::WHITE, .75f, font);
	g_renderer->DrawText2D(Vector2(3.f, 6.5f), stamTxt, 1.f, Rgba::WHITE, .75f, font);
	g_renderer->DrawText2D(Vector2(3.f, 5.5f), speedTxt, 1.f, Rgba::WHITE, .75f, font);
	g_renderer->DrawText2D(Vector2(3.f, 4.5f), moveTxt, 1.f, Rgba::WHITE, .75f, font);
	g_renderer->DrawText2D(Vector2(3.f, 3.5f), attTxt, 1.f, Rgba::WHITE, .75f, font);
	g_renderer->DrawText2D(Vector2(3.f, 2.5f), defTxt, 1.f, Rgba::WHITE, .75f, font);
	g_renderer->DrawText2D(Vector2(3.f, 1.5f), magTxt, 1.f, Rgba::WHITE, .75f, font);
}
