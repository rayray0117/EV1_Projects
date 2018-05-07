//Breaking this into its own class so code will be easier for me to read.
#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
//////////////////////////////////////////////////////////////////////////
class GameState
{
public:
	GameState() {};
	~GameState() {};
	
	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual void DebugRender() const {}
};