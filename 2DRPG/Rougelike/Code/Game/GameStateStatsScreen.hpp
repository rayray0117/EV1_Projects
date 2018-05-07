#pragma once
#include "Game/GameState.hpp"
//////////////////////////////////////////////////////////////////////////
class GameStateStatsScreen : public GameState
{
public:
	GameStateStatsScreen();
	~GameStateStatsScreen();
	
	void Update(float deltaSeconds) override;
	void Render() const override;
public:
	World* mo_world;
	Camera2D* mo_camera;

private:
	void Render_StatsScreen() const;
};