#include "Game/GameStatePlay.hpp"
#include "Game/World.hpp"
#include "Game/Camera2D.hpp"
#include "Game/Map.hpp"
#include "Game/Controller.hpp"
//////////////////////////////////////////////////////////////////////////
GameStatePlay::GameStatePlay()
{
	mo_world = g_theGame->m_theWorld;
	mo_camera = g_theGame->m_theCamera;
	
	Map* currentMap = getCurrentMap();
	PlayMusic(currentMap->m_bgm);
}

GameStatePlay::~GameStatePlay()
{

}
//////////////////////////////////////////////////////////////////////////
void GameStatePlay::Update(float deltaSeconds)
{
	if (wasInputJustPressed(INPUT_START))
	{
		g_theGame->ChangeGameState(GAMESTATE_MAINMENU);
		return;
	}
	else if (wasInputJustPressed(INPUT_STATSCREEN))
	{
		g_theGame->ChangeGameState(GAMESTATE_STATS);
		return;
	}

	//Update Real game stuff
	mo_world->Update(deltaSeconds);
}

void GameStatePlay::Render() const
{
	//Render camera's view.
	mo_camera->Render();

	//Begin rendering real game stuff
	mo_world->Render();

	//Draw the player controllers stuff;
	Controller::s_controllers[0]->Render();
}

void GameStatePlay::DebugRender() const
{
	mo_world->DebugRender();
}
