#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/RHI.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/RHI/Sampler.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Renderer/MeshGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ObjLoader.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/RHI/RasterState.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/ParticleEmitter.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/ParticleManager.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Networking/Net.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Networking/TCPSession.hpp"
#include "Engine/Networking/RemoteCommandService.hpp"
#include "Game/GSMainMenu.hpp"
#include "Game/GSHost.hpp"
#include "Game/GSJoin.hpp"
#include "Game/World.hpp"
#include "GSClient.hpp"
#include "GameCommon.hpp"
#include "GSHostMenu.hpp"
//////////////////////////////////////////////////////////////////////////
float cameraLightIntensity = .25f;
float cameraLightAngle = 30;
ParticleSystem* g_partSys = nullptr;
bool g_showMemTrac = false;
Mesh* g_cube = nullptr;
Texture2D* g_cubeTexture = nullptr;
const float ZOOM_SCALE = .5f;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
Game::Game()
	: m_gamePaused(false)
	, m_debugMode(false)
{
	loadConfigTexturesAndShaders();
	
	g_renderer->timeData.game_frame_time = 0.f;
	g_renderer->timeData.game_time = 0.f;

	g_renderer->CreateOrGetFont("Data/Fonts/Garamound32.fnt");
	

	SetupParticleStuff();

	Profiler::GetInstance()->bDrawWithDropShadow = true;
	
	m_consoleSpriteSheet = new SpriteSheet(g_renderer->CreateOrGetTexture("Data/Config/PP-TV.png"), 3, 4);
	g_theConsole->m_anim = new SpriteAnim(*m_consoleSpriteSheet, 0, 11, 5.f, SPRITE_ANIM_MODE_LOOPING);
	g_theConsole->m_anim->Resume();

	NetSystemStartup();
	RCS->setup_message_definition();

	Texture2D* blockSprites = g_renderer->CreateOrGetTexture("Data/Images/SimpleMinerAtlas.png");
	m_blockSpriteSheet = new SpriteSheet(blockSprites, 16, 16);
	g_renderer->SetBlendFunc(RHIEnum::BLEND_SOURCE_ALPHA, RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA);
	g_renderer->SetRenderTarget(nullptr, nullptr);
	g_renderer->BindShader(DEFAULT_UNLIT_SHADER);
	g_renderer->EnableBackFaceCulling(true);

	m_world = new World();
	g_audioSystem->PlayMusic(g_musicPath);
}

Game::~Game() 
{
	SAFE_DELETE(g_cube);
	SAFE_DELETE(g_cubeTexture);
	SAFE_DELETE(m_world);
	SAFE_DELETE(m_currentGameState);
	NetSystemShutdown();
}

//////////////////////////////////////////////////////////////////////////
void Game::Update(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	UpdateCheats(deltaSeconds);
	UpdateTimeBuffer(deltaSeconds);

	if (m_gamePaused)
		return;

	PARTICLE_MANAGER->Update(deltaSeconds);
	RCS->Update();


	if(m_currentGameState)
		m_currentGameState->Update(deltaSeconds);
}

void Game::Render() const
{
	PROFILE_SCOPE_FUNCTION();
	g_renderer->ClearScreen(Rgba::FORESTGREEN);
	g_renderer->ClearDepth();
	//g_renderer->BindShader(DEFAULT_UNLIT_SHADER);
	g_renderer->SetViewport(0, 0, g_windowWidth, g_windowHeight);
	//g_renderer->pushMatrix();

	if (m_currentGameState)
		m_currentGameState->Render();

	//g_renderer->popMatrix();

//	PARTICLE_MANAGER->RenderAll(g_renderer);

	if (m_debugMode)
	{
		Profiler::GetInstance()->Render(g_renderer);
	}
}
//////////////////////////////////////////////////////////////////////////
void Game::ChangeGameState(EGameStateType state)
{
	SAFE_DELETE(m_currentGameState);
	switch (state)
	{
	case GAMESTATE_MAINMENU: m_currentGameState = new GSMainMenu();
		break;
	case GAMESTATE_HOST: m_currentGameState = new GSHost();
		break;
	case GAMESTATE_JOIN: m_currentGameState = new GSJoin();
		break;
	case GAMESTATE_CLIENT: m_currentGameState = new GSClient();
		break;
	case GAMESTATE_HOST_MENU: m_currentGameState = new GSHostMenu();
		break;
	case GAMESTATE_PLAY: 
		//break;
	case NUM_GAMESTATES:
	default:  m_currentGameState = nullptr; 
		break;
	}

	ASSERT_OR_DIE(nullptr != m_currentGameState, "GameState set to null");
}

//////////////////////////////////////////////////////////////////////////
void Game::SetupParticleStuff()
{
	ParticleEmitter* emitter = new ParticleEmitter();
	emitter->m_emissionRate = 2.f;
	emitter->setSpeed(100.f);
	emitter->m_direction =Vector3(0.f, 1.f, 1.f);
	emitter->setSize(Vector3::ONE);
	emitter->setLifetime( 4.75f);
	emitter->setTint(Rgba::BLACK, Rgba::WHITE);
	emitter->m_transform.setFromMatrix(Matrix4());
	emitter->m_transform.scale *= .01f; //Scale emitter down to centimeters
	emitter->setMaxNumberOfParticles(10);
	emitter->bPrewarm = false;
	emitter->m_gravity = Vector3(0.f, 5.f, 0.f);
	emitter->m_tangentialAcceleration = 50.f;
	static SQT trans;
	trans.setFromMatrix(Matrix4());

	ParticleSystemDefinition::ParseFromFile("Data/Particles/Particles.xml");

}

void Game::PlayRandomSound(const std::string& path, int numberOfSounds)
{
	if (numberOfSounds == 0)
		return;

	int soundNumber = getRandomIntInRange(1, numberOfSounds);
	std::string soundToPlay = path + std::to_string(soundNumber) + ".ogg";

	g_audioSystem->PlaySoundFX(soundToPlay, g_sfxVolume);
}

//////////////////////////////////////////////////////////////////////////
void Game::UpdateCheats(float& deltaSeconds)
{
	if (g_inputSystem->isKeyDown('T'))
	{
		deltaSeconds *= 0.1f;
		if (g_inputSystem->isKeyDown(KEYCODE_SHIFT))
			deltaSeconds *= 0.1f;
	}
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_PAUSE))
	{
		m_gamePaused = !m_gamePaused;
		if (m_gamePaused)
		{
			Profiler::GetInstance()->Pause();
		}
		else
		{
			Profiler::GetInstance()->Resume();
		}
	}
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_F1))
		m_debugMode = !m_debugMode;

	if (g_inputSystem->wasKeyJustPressed('0'))
	{
		static SQT trans;
		trans.setFromMatrix(Matrix4());
		PARTICLE_MANAGER->PlayEffect_OneOff("space", trans, g_renderer);
	}
}

void Game::UpdateTimeBuffer(float deltaSeconds)
{
	g_renderer->timeData.game_frame_time = deltaSeconds;
	g_renderer->timeData.game_time += deltaSeconds;
	g_renderer->time_cb->update(g_renderer->context, &g_renderer->timeData);
}
//////////////////////////////////////////////////////////////////////////
void Game::loadConfigTexturesAndShaders()
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
////////////////////////////////////////////////////////////////////////// //////////////////////////////////////////////////////////////////////////
bool TperformCurrentCharacterState(unsigned int& pos, const std::vector<unsigned char>& buffer, std::vector<unsigned char>& writeBuffer)
{
	const unsigned char c = buffer[pos];

	if (c == '"')
	{
		std::string quote = getQuoteWithQuote(pos, buffer);
		for (uint i = 0; i < quote.length(); ++i)
		{
			writeBuffer.push_back(quote[i]);
		}
		return true;
	}

	if ((pos < buffer.size()))
	{
		if ((pos + 1 < buffer.size()))
		{
			if (isCStyleCommentBegin(c, buffer[pos + 1]))
			{
				skipCStyleComment(pos, buffer);
				return true;
			}
			if (isCPPStyleComment(c, buffer[pos + 1]))
			{
				skipLine(pos, buffer);
				return true;
			}
		}
		
		{
			writeBuffer.push_back(c);
			++pos;
			return true;
		}
	}

	

	return true; // Don't know how you got here but it's all good
}

bool TryFileToParse(const char* filename)
{
	bool success = true;

	std::vector<unsigned char> buffer;
	success &= ReadBufferToFile(buffer, filename);

	if (!success)
		return false;

	std::vector<unsigned char> write_buffer;
	write_buffer.reserve(buffer.size());

	unsigned int bufferIndex = 0;

	for (; bufferIndex < buffer.size(); /*++bufferIndex*/)
	{
		success &= TperformCurrentCharacterState(bufferIndex, buffer, write_buffer);
	}

	WriteBufferToFile(write_buffer, filename);

	return success;
}

/*/
COMMAND(parse_out_comments, "Params: Filename")
{
	TryFileToParse(arguments.c_str());
}
//*/

COMMAND(set_seed, "Params: int. Sets the world seed for hosting a game.")
{
	if (g_theGame->m_currentGameState->m_state != GAMESTATE_HOST_MENU)
	{
		g_theConsole->addMessage("Can't call this command outside of the Host Menu!", Rgba::RED);
		return;
	}
	g_Seed = std::stoi(arguments);
}

COMMAND(music, "Type the name of the music file you want. Must already be in Music folder!")
{
	g_musicPath = std::string("Data/Audio/Music/" + arguments).c_str();
	g_audioSystem->PlayMusic(g_musicPath, g_musicVolume);
}

COMMAND(music_volume, "sets the volume of the music. range: 0-1")
{
	g_musicVolume = std::stof(arguments);
}