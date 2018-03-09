#pragma once
#include "Engine/Audio/Audio.hpp"
#include <vector>
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class SpriteSheet;
class GameState;
class World;
//////////////////////////////////////////////////////////////////////////
enum EGameStateType
{
	GAMESTATE_MAINMENU,
	GAMESTATE_PLAY,
	GAMESTATE_HOST,
	GAMESTATE_JOIN,
	GAMESTATE_CLIENT,
	GAMESTATE_HOST_MENU,
	NUM_GAMESTATES
};
//////////////////////////////////////////////////////////////////////////
class Game
{
public:
	Game();
	~Game();
	void Update(float deltaSeconds);
	void Render() const;

	void ChangeGameState(EGameStateType state);

	void SetupParticleStuff();

public:
	bool m_debugMode;
	bool m_gamePaused;

	GameState* m_currentGameState = nullptr;
	World* m_world = nullptr;
	SpriteSheet* m_blockSpriteSheet = nullptr;

	void PlayRandomSound(const std::string& path, int numberOfSounds);
private:
	void UpdateCheats(float& deltaSeconds);
	void UpdateTimeBuffer(float deltaSeconds);

private:
	void loadConfigTexturesAndShaders();
private:
	std::vector<std::string> textureList;
	std::vector<std::string> shaderList;

	SpriteSheet* m_consoleSpriteSheet;
};
