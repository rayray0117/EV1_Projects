#pragma once
#include "Game/GameState.hpp"
//////////////////////////////////////////////////////////////////////////
class GameStateNewGameMenu : public GameState
{
public:
	GameStateNewGameMenu();


	~GameStateNewGameMenu() {}

	void Update(float deltaSeconds) override;
	void Render() const override;
public:

private:
	void HandleInput();
	void drawMenuItems() const;


	//Option stuff
	void DecrementOption();
	void IncrementOption();
	void LoadCurrentOption();
	uint m_currentAdventureOption = 0;
	
	void SetupAdventureNames();
};