#pragma once
#include "Game/GameState.hpp"
#include "Engine/Networking/Net.hpp"
//////////////////////////////////////////////////////////////////////////
class GSJoin : public GameState
{
public:
	GSJoin();
	~GSJoin();
	

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void HandleInput() override;
	void FinishJoin(Net::luid id, int worldSize);
public:
	std::string m_hostAddress;
	std::string m_username;

private:
	void CallOption();
	void TryToJoin();
	void BeginJoin();
	void drawMenuItems() const;
	void CallSetName();
	void CallSetHost();
private:
	uint m_optionIndex = 0;
};