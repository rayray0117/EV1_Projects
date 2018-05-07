#pragma once
#include "Game/GameState.hpp"
//////////////////////////////////////////////////////////////////////////
class World;
class Camera2D;
//////////////////////////////////////////////////////////////////////////
class GameStatePlay : public GameState
{
public:
	GameStatePlay();
	~GameStatePlay();
	
	void Update(float deltaSeconds) override;
	void Render() const override;
	void DebugRender() const override;
public:
	World* mo_world;
	Camera2D* mo_camera;
};