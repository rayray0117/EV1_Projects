#pragma once
#include "Game/GameState.hpp"
//////////////////////////////////////////////////////////////////////////
class PlayerInfo;
class Camera3D;
//////////////////////////////////////////////////////////////////////////
class GSHost : public GameState
{
public:
	GSHost();
	~GSHost();
	
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	virtual void HandleInput() override;
public:
	uint8 m_playerInfoNumberToWatch = 0;
	Camera3D* m_camera;
private:
	void drawHostingInfo() const;
	void drawPlayerInfoList() const;
	void drawPlayerInfo(PlayerInfo* player) const;
};