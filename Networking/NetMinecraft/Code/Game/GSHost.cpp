#include "Game/GSHost.hpp"
#include "Engine/Networking/TCPSession.hpp"
#include "Engine/Networking/Net.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Game/World.hpp"
#include "Engine/Networking/NetConnection.hpp"
#include "GameSession.hpp"
#include "GameCommon.hpp"
#include "PlayerInfo.hpp"
#include "Game.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Actor.hpp"
#include "Game/Camera3D.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/Command.hpp"
//////////////////////////////////////////////////////////////////////////
Net::portId GAME_HOST_PORT = 28210;
//////////////////////////////////////////////////////////////////////////
GSHost::GSHost()
{
	m_state = GAMESTATE_HOST;

	g_gameSession = new GameSession();

	g_gameSession->Host(GAME_HOST_PORT);
	g_gameSession->my_player = nullptr;

	g_renderer->SetSampler(RHIEnum::FILTER_POINT, RHIEnum::FILTER_POINT);
	g_renderer->SetBlendFunc(RHIEnum::BLEND_SOURCE_ALPHA, RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA);
	g_renderer->BindShader(DEFAULT_UNLIT_SHADER);
	g_renderer->EnableBackFaceCulling(true);
	g_renderer->SetAmbientLight(1.f, Rgba::WHITE);
	m_camera = new Camera3D();
	m_camera->m_position = Vector3(0.f, 0.f, 2.f);
	//loadTidus();
}

GSHost::~GSHost()
{
	SAFE_DELETE(g_gameSession);
}
//////////////////////////////////////////////////////////////////////////
void GSHost::Update(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	g_gameSession->Update(deltaSeconds);
	HandleInput();
	
	g_theGame->m_world->Update(deltaSeconds);
	m_camera->UpdateCameraFromMouse(deltaSeconds);
}

void GSHost::Render() const
{
	PROFILE_SCOPE_FUNCTION();
	g_renderer->pushMatrix();
	g_renderer->ClearScreen(g_skyColor);
	g_renderer->ClearDepth();
	m_camera->Render();
	g_renderer->EnableBackFaceCulling(false);
	g_renderer->EnableDepthTesting();
	g_renderer->EnableDepthWriting();
	g_renderer->SetDepthFunc(RHIEnum::DEPTH_LESS);
	g_renderer->BindTexture(&g_theGame->m_blockSpriteSheet->getTextureDX());
	g_theGame->m_world->Render();
	//drawTidus();
	g_renderer->popMatrix();
	g_renderer->SetOrthoProjection(Vector2::ZERO, Vector2(160, 90));
	drawHostingInfo();
}

void GSHost::HandleInput()
{
	if (g_inputSystem->isKeyDown(KEYCODE_ADD))
	{
		
	}
	if (g_inputSystem->isKeyDown(KEYCODE_SUBTRACT))
	{
		
	}
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_0))
	{
		g_gameSession->my_player = nullptr;
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_1))
	{
		if (g_gameSession->m_players.empty())
			g_gameSession->my_player = nullptr;
		m_playerInfoNumberToWatch = m_playerInfoNumberToWatch + 1;
		if (m_playerInfoNumberToWatch > g_gameSession->m_players.size())
			m_playerInfoNumberToWatch = 0;
		g_gameSession->my_player = g_gameSession->getPlayerInfo(m_playerInfoNumberToWatch);
	}
}

//////////////////////////////////////////////////////////////////////////
void GSHost::drawHostingInfo() const
{

	std::string strAddr = Stringf("Join Address: %s, Chunks = %i, RLEs = %i", NetAddressToString(GetMyAddress(GAME_HOST_PORT)).c_str(), g_theGame->m_world->m_chunks.size(), g_theGame->m_world->m_rleChunks.size());

	g_renderer->DrawText2D(Vector2(0, 88), "HOSTING", 2.f, Rgba::YELLOW, .75);
	g_renderer->DrawText2D(Vector2(0, 86), strAddr, 2.f, Rgba::WHITE, .75);

	g_renderer->translate2D(0.f, 84.f);

	drawPlayerInfoList();
}

void GSHost::drawPlayerInfoList() const
{
	std::string playerAmountTxt = Stringf("Players (%i/%i)", g_gameSession->m_players.size(), g_gameSession->m_session->m_maxConnectionCount);
	g_renderer->DrawText2D(Vector2(0, 0), playerAmountTxt, 2.f, Rgba::GRAY_192, .75);
	g_renderer->translate2D(0.f, -2.f);

	for (uint i = 0; i < g_gameSession->m_players.size(); ++i)
	{
		drawPlayerInfo(g_gameSession->m_players[i]);
		g_renderer->translate2D(0.f, -2.f);
	}
}

void GSHost::drawPlayerInfo(PlayerInfo* player) const
{
	if (player == nullptr)
		return;

	NetConnection* playerConnection = g_gameSession->m_session->getConnection(player->m_id);
	if (playerConnection == nullptr)
		return;

	std::string strAddr = Stringf("- %i %s. %s", player->m_id, player->m_name.c_str(), NetAddressToString(playerConnection->address).c_str());
	g_renderer->DrawText2D(Vector2::ZERO, strAddr, 2.f, Rgba::GRAY_192, .75);

	g_renderer->translate2D(0.f, -2.f);
	std::string inputStr = Stringf("  - Left: %i  Right: %i  Up: %i  Down: %i", player->m_left, player->m_right, player->m_up, player->m_down);
	g_renderer->DrawText2D(Vector2::ZERO, inputStr, 2.f, Rgba::GRAY_192, .75);
}

COMMAND(set_farclip, "")
{
	g_farClip = std::stof(arguments);
}