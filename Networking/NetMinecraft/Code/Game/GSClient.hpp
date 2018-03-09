#pragma once
#include "Game/GameState.hpp"
//////////////////////////////////////////////////////////////////////////
class PlayerInfo;
//////////////////////////////////////////////////////////////////////////
class GSClient : public GameState
{
public:
	GSClient();
	~GSClient();
	
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;


	virtual void HandleInput(float deltaSeconds);

	void BlockTypeInput();
	void BlockPlaceRemoveInput();
public:
	

private:
	void drawClientInfo() const;
	void drawPlayerInfoList() const;
	void drawPlayerInfo(PlayerInfo* player) const;
	void drawCrosshair() const;
	void drawPlayerBlockSelectUI(int selectedBlock) const;
private:

};