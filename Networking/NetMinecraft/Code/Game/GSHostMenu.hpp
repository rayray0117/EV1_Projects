#pragma once
#include "Game/GameState.hpp"
//////////////////////////////////////////////////////////////////////////
class GSHostMenu : public GameState
{
public:
	GSHostMenu();
	~GSHostMenu();
	void Update(float deltaSeconds) override;
	void Render() const override;
	void HandleInput() override;
public:

private:

	void CallOption();
	void drawMenuItems() const;
	void CallSetSeedCmd();
	void CallSetSaveCmd();
private:
	uint m_optionIndex = 0;
};