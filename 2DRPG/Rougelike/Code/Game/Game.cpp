#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/RHI/Sampler.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#include "Game/TileDefinition.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/FeatureDefinition.hpp"
#include "Game/PawnDefinition.hpp"
#include "Game/AdventureDefinition.hpp"

#include "Game/GameState.hpp"
#include "Game/GameStateMainMenu.hpp"
#include "Game/GameStatePlay.hpp"
#include "Game/GameStateStatsScreen.hpp"

#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Game/PlayerController.hpp"
#include "Game/Pawn.hpp"
#include "Game/AIController.hpp"
#include "Game/Feature.hpp"
#include "Game/Camera2D.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "GameStateNewGameMenu.hpp"
//////////////////////////////////////////////////////////////////////////
Game::Game()
{
    // Begin Game setup
    Controller::s_controllers.push_back(new PlayerController());
    Controller::s_controllers.push_back(new AIController());

	LoadInDefinitions();
	
	m_theCamera = new Camera2D();
	//CreateAndLoadDefaultMap();
	//End Game setup

	// Do rendering setup after game setup so D3D11 errors don't show up when loading a bad XML file.
	// Begin Rendering Setup
	LoadConfigTexturesAndShaders();

	g_renderer->BindShader(DEFAULT_UNLIT_SHADER);
	g_theConsole->m_haveConsolePrepareRendering = true;
	g_renderer->SetBlendFunc(RHIEnum::BLEND_SOURCE_ALPHA, RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA);
	g_renderer->EnableDepthTesting(false);
	g_renderer->EnableDepthWriting(false);
	g_renderer->SetViewport(0, 0, g_windowWidth, g_windowHeight);
	
	//Set up consoles fun gif image!
	m_consoleSpriteSheet = new SpriteSheet(g_renderer->CreateOrGetTexture("Data/Config/025Pikachuanimated2.png"), 2, 1);
	g_theConsole->m_anim = new SpriteAnim(*m_consoleSpriteSheet, 0, 1, 1.f, SPRITE_ANIM_MODE_LOOPING);
	g_theConsole->m_anim->Resume();

	//Create Mesh builders for the three render layers
	m_fills = new MeshBuilder();
	m_overlays = new MeshBuilder();
	m_glyphs = new MeshBuilder();
	// End Rendering Setup
}

Game::~Game() 
{
	SAFE_DELETE(m_currentGameState);
    SAFE_DELETE(m_theWorld);
	SAFE_DELETE(m_theCamera);
	SAFE_DELETE(m_consoleSpriteSheet);
	SAFE_DELETE(m_fillMesh);
	SAFE_DELETE(m_glyphMesh);
	SAFE_DELETE(m_overlayMesh);
	SAFE_DELETE(m_fills);
	SAFE_DELETE(m_glyphs);
	SAFE_DELETE(m_overlays);
}

////////////////////////////////////////////////////////////////////////// Public Functions //////////////////////////////////////////////////////////////////////////
void Game::Update(float deltaSeconds)
{
	UpdateCheats(deltaSeconds);

	g_audioSystem->Update();

	if (m_gamePaused)
		return;
	
	m_currentGameState->Update(deltaSeconds);
}

void Game::Render() const
{
	m_currentGameState->Render();

	if (m_debugMode)
	{
		m_currentGameState->DebugRender();
	}
}

void Game::Redraw()
{
	if (m_theWorld == nullptr) return;

	m_fills->clear();
	m_fills->begin(RHIEnum::PRIMATIVE_QUADS, false);
	m_glyphs->clear();
	m_glyphs->begin(RHIEnum::PRIMATIVE_QUADS, false);
	m_overlays->clear();
	m_overlays->begin(RHIEnum::PRIMATIVE_QUADS, false);
	m_theWorld->Redraw();
	m_fills->end();
	m_glyphs->end();
	m_overlays->end();

	SAFE_DELETE(m_fillMesh);
	SAFE_DELETE(m_glyphMesh);
	SAFE_DELETE(m_overlayMesh);

	m_fillMesh = g_renderer->CreateAndGetMeshStatic(m_fills);
	m_glyphMesh = g_renderer->CreateAndGetMeshStatic(m_glyphs);
	m_overlayMesh = g_renderer->CreateAndGetMeshStatic(m_overlays);
}
//////////////////////////////////////////////////////////////////////////
void Game::ChangeGameState(EGameStateType state)
{
	SAFE_DELETE(m_currentGameState);
	switch (state)
	{
	case GAMESTATE_MAINMENU: m_currentGameState = new GameStateMainMenu();
		break;
	case GAMESTATE_PLAY: m_currentGameState = new GameStatePlay();
		break;
	case GAMESTATE_STATS: m_currentGameState = new GameStateStatsScreen();
		break;
	case GAMESTATE_NEWGAMEMENU: m_currentGameState = new GameStateNewGameMenu();
		break;
	case GAMESTATE_PATHING:
		break;
	case GAMESTATE_GENERATING:
		break;
	case NUM_GAMESTATES:
		break;
	default:
		break;
	
	}
}

void Game::LoadNewAdventure(const std::string& adventureName)
{
	SAFE_DELETE(m_theWorld);
	m_theWorld = new World();
	m_theWorld->CreateWorld(adventureName);
	m_currentAdventure = adventureName;
	for (uint i = 0; i < Controller::s_controllers.size(); i++)
	{
		Controller::s_controllers[i]->Reset();
	}
	//ChangeGameState(GAMESTATE_PLAY);
}

////////////////////////////////////////////////////////////////////////// Private Functions //////////////////////////////////////////////////////////////////////////
void Game::UpdateCheats(float& deltaSeconds)
{
	if (g_inputSystem->isKeyDown('T'))
	{
		deltaSeconds *= 0.1f;
		if (g_inputSystem->isKeyDown(KEYCODE_SHIFT))
			deltaSeconds *= 0.1f;	
	}
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_PAUSE))
		m_gamePaused = !m_gamePaused;
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_F1))
		m_debugMode = !m_debugMode;
}
////////////////////////////////////////////////////////////////////////// Game SetUp functions //////////////////////////////////////////////////////////////////////////
void Game::LoadInDefinitions()
{
	TileDefinition::ParseFromFile(XML_TILEDEF_FILEPATH);
	ItemDefinition::ParseFromFile(XML_ITEMDEF_FILEPATH);
	PawnDefinition::ParseFromFile(XML_PAWNDEF_FILEPATH);
	FeatureDefinition::ParseFromFile(XML_FEATUREDEF_FILEPATH);
	MapDefinition::ParseFromFile(XML_MAPDEF_FILEPATH);
	AdventureDefinition::ParseFromFile(XML_ADVENDEF_FILEPATH);
}

void Game::CreateAndLoadDefaultMap()
{
	m_theWorld = new World();
	m_theWorld->AddMap(new Map(MapDefinition::getDefinition("Basic_1")), "DefaultMap");
	m_theWorld->LoadMap("DefaultMap", false);

	Map* currentMap = m_theWorld->mo_currentMap;
	Pawn* pawn = new Pawn("player", 0);
	pawn->MoveToTile(currentMap->getTile(IntVector2(1, 1)));
	pawn->currentHP = pawn->m_baseStats.m_stats[STAT_HEALTH];
	pawn->currentStamina = pawn->m_baseStats.m_stats[STAT_STAMINA];
	currentMap->addPawn(pawn);

	pawn = new Pawn("goblin", 1);
	pawn->MoveToTile(currentMap->getTile(IntVector2(7, 7)));
	pawn->currentHP = pawn->m_baseStats.m_stats[STAT_HEALTH];
	pawn->currentStamina = pawn->m_baseStats.m_stats[STAT_STAMINA];
	currentMap->addPawn(pawn);

	pawn = new Pawn("wanderling", 1);
	pawn->MoveToTile(currentMap->getTile(IntVector2(6, 7)));
	pawn->currentHP = pawn->m_baseStats.m_stats[STAT_HEALTH];
	pawn->currentStamina = pawn->m_baseStats.m_stats[STAT_STAMINA];
	currentMap->addPawn(pawn);

	Feature* feature = new Feature("box");
	Tile* featureTile = currentMap->getTile(IntVector2(1, 8));
	feature->mo_currentTile = featureTile;
	featureTile->mo_feature = feature;
	feature->currentHP = 13;
	currentMap->m_actors.push_back(feature);
}
////////////////////////////////////////////////////////////////////////// Rendering Stuff Ignore //////////////////////////////////////////////////////////////////////////
void Game::LoadConfigTexturesAndShaders()
{
	// Load in textures
	for (int textureIndex = 0; textureIndex < g_numImages; ++textureIndex)
	{
		std::string textureFilePath;
		std::string image_key = IMAGE_KEY + std::to_string(textureIndex);

		bool success = g_gameConfig->ConfigGetString(image_key.c_str(), textureFilePath);
		ASSERT_RECOVERABLE(success, Stringf("WARNING: Texture #%i was not found!", textureIndex).c_str());
		if (success)
			textureList.push_back(textureFilePath);


		g_renderer->CreateOrGetTexture(textureFilePath);
	}

	// Load in shaders
	for (int shaderIndex = 0; shaderIndex < g_numShaders; ++shaderIndex)
	{
		std::string shaderFilePath;
		std::string shader_key = SHADER_KEY + std::to_string(shaderIndex);

		bool success = g_gameConfig->ConfigGetString(shader_key.c_str(), shaderFilePath);
		ASSERT_RECOVERABLE(success, Stringf("WARNING: Shader #%i was not found!", shaderIndex).c_str());
		if (success)
			shaderList.push_back(shaderFilePath);

		g_renderer->CreateOrGetShader(shaderFilePath);
	}
}
////////////////////////////////////////////////////////////////////////// COMMANDS //////////////////////////////////////////////////////////////////////////
COMMAND(music_play, "parameters: File Path, volume(optional)")
{
	std::vector<std::string> args;
	splitArgs(arguments, args);
	if (args.size() > 1)
	{
		PlayMusic(args[0], stof(args[1]));
	}
	else
	{
		PlayMusic(args[0]);
	}
}

COMMAND(adventure, "Name an adventure to load. Don't run while playing in world cause I don't know what'll happen!")
{
	g_theGame->LoadNewAdventure(arguments);
}