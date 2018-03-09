#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Game/GameState.hpp"
//////////////////////////////////////////////////////////////////////////
class GSMainMenu : public GameState
{
public:
	GSMainMenu();
	~GSMainMenu();

	void Update(float deltaSeconds) override;
	void Render() const override;
	void HandleInput() override;
public:

private:
	
	void CallOption();
	void drawMenuItems() const;

private:
	uint m_optionIndex = 0;
};