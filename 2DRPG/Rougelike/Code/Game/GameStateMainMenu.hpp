#pragma once
#include "Game/GameState.hpp"
//////////////////////////////////////////////////////////////////////////
class GameStateMainMenu : public GameState
{
public:
	GameStateMainMenu();
	~GameStateMainMenu();
	
	void Update(float deltaSeconds) override;
	void Render() const override;
public:
	
private:
	void HandleInput();
	void drawMenuItems() const;
};