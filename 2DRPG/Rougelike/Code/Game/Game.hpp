#pragma once
#include "Engine/Audio/Audio.hpp"
#include "Game/GameStateType.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class World;
class Camera2D;
class GameState;
class SpriteSheet;
class MeshBuilder;
class Mesh;
//////////////////////////////////////////////////////////////////////////
class Game
{
public:
	Game();
	~Game();

	void Update(float deltaSeconds);
	void Render() const;
	void Redraw();

	void ChangeGameState(EGameStateType state);
	void LoadNewAdventure(const std::string& adventureName);
	
public:
	bool m_debugMode = true;
	bool m_gamePaused = false;

	World* m_theWorld = nullptr;
	Camera2D* m_theCamera = nullptr;
	GameState* m_currentGameState = nullptr;

	//Fills is the mesh for the current background color square to draw.
	MeshBuilder* m_fills = nullptr;
	Mesh* m_fillMesh = nullptr;
	//Glyph is the mesh for the current "letters/chars" and their color to draw.
	MeshBuilder* m_glyphs = nullptr;
	Mesh* m_glyphMesh = nullptr;
	//Overlays typically represent current fog of war values by covering known tiles with a certain level of transparent gray square.
	MeshBuilder* m_overlays = nullptr;
	Mesh* m_overlayMesh = nullptr;

	std::string m_currentAdventure = "None";
private:
	void UpdateCheats(float& deltaSeconds);

	void LoadInDefinitions();
	void CreateAndLoadDefaultMap();


// Rendering Stuff to ignore
private:
	void LoadConfigTexturesAndShaders();
private:
	std::vector<std::string> textureList;
	std::vector<std::string> shaderList;

	SpriteSheet* m_consoleSpriteSheet;
};