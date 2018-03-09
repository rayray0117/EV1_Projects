#include "Game/GSClient.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Networking/Net.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"	
#include "Engine/Core/Rgba.hpp"
#include "Game/GameSession.hpp"
#include "Game/PlayerInfo.hpp"
#include "Engine/Networking/TCPSession.hpp"
#include "Game/World.hpp"
#include "Engine/Networking/NetMessage.hpp"
#include "GameSession.hpp"
#include "GameCommon.hpp"
#include "PlayerInfo.hpp"
#include "Game.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Actor.hpp"
#include "Game/Camera3D.hpp"
#include "Engine/Core/Profiler.hpp"
//////////////////////////////////////////////////////////////////////////
GSClient::GSClient()
{
}

GSClient::~GSClient()
{
	SAFE_DELETE(g_gameSession);
}
//////////////////////////////////////////////////////////////////////////
void GSClient::Update(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	g_gameSession->Update(deltaSeconds);
	HandleInput(deltaSeconds);
	g_theGame->m_world->Update(deltaSeconds);
	
}

void GSClient::Render() const
{
	PROFILE_SCOPE_FUNCTION();
	g_renderer->pushMatrix();
	g_renderer->ClearScreen(g_skyColor);
	g_renderer->ClearDepth();
	if(g_gameSession->getCamera())
		g_gameSession->getCamera()->Render();
	g_renderer->EnableBackFaceCulling();
	g_renderer->EnableDepthTesting();
	g_renderer->EnableDepthWriting();
	g_renderer->SetDepthFunc(RHIEnum::DEPTH_LESS);
	g_renderer->BindTexture(&g_theGame->m_blockSpriteSheet->getTextureDX());
	g_theGame->m_world->Render();
	g_renderer->popMatrix();
	if (g_gameSession->my_player != nullptr)
	{
		drawPlayerBlockSelectUI((int)g_gameSession->my_player->m_placementBlockType);
	}
	drawCrosshair();
	g_renderer->SetOrthoProjection(Vector2::ZERO, Vector2(160, 90));
	drawClientInfo();
}

void GSClient::HandleInput(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	UNUSED(deltaSeconds);
	/*/
	Actor* player = g_gameSession->getMyPlayer();
	if (player)
	{
		player->HandleInput(deltaSeconds);
	}
	//*/
	
	/*/
	if (g_gameSession->getMyPlayer() != nullptr)
	{
		g_gameSession->getMyPlayer()->m_camera->UpdateCameraFromMouse(deltaSeconds);
		NetMessage msg(GAMEMSG_CAMERA_UPDATE);
		msg.sender = g_gameSession->m_session->my_connection;
		msg.write(g_gameSession->getCamera()->m_degreesAboutZ);
		msg.write(g_gameSession->getCamera()->m_degreesAboutY);
		g_gameSession->m_session->SendMessageToOthers(msg);
	}
	//*/


	NetMessage msg(GAMEMSG_INPUT_UPDATE);
	msg.sender = g_gameSession->m_session->my_connection;
	bool shouldSendMsg = false;
	std::string msgStr = "";
	if (wasInputJustPressed(INPUT_UP))
	{
		shouldSendMsg = true;
		msgStr += "u 1";
	}
	else if (wasInputJustReleased(INPUT_UP))
	{
		shouldSendMsg = true;
		msgStr +=  "u 0";
	}
	if (wasInputJustPressed(INPUT_DOWN))
	{
		shouldSendMsg = true;
		msgStr +=  " d 1";
	}
	else if (wasInputJustReleased(INPUT_DOWN))
	{
		shouldSendMsg = true;
		msgStr +=  " d 0";
	}
	if (wasInputJustPressed(INPUT_RIGHT))
	{
		shouldSendMsg = true;
		msgStr += " r 1";
	}
	else if (wasInputJustReleased(INPUT_RIGHT))
	{
		shouldSendMsg = true;
		msgStr += " r 0";
	}
	if (wasInputJustPressed(INPUT_LEFT))
	{
		shouldSendMsg = true;
		msgStr += " l 1";
	}
	else if (wasInputJustReleased(INPUT_LEFT))
	{
		shouldSendMsg = true;
		msgStr += " l 0";
	}
	if (wasInputJustPressed(INPUT_X))
	{
		//jump(.16f);
	}

	if (shouldSendMsg)
	{
		msg.write_string(msgStr);
		g_gameSession->m_session->SendMessageToOthers(msg);
	}

	BlockTypeInput();
	BlockPlaceRemoveInput();
}

void GSClient::BlockTypeInput()
{
	if (g_gameSession->my_player == nullptr) return;

	//BlockTypes prevBlockType = g_gameSession->my_player->m_placementBlockType;
	BlockTypes& blockType = g_gameSession->my_player->m_placementBlockType;

	if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD1) || g_inputSystem->wasKeyJustPressed('1'))
	{
		blockType = GRASS;
		g_theGame->PlayRandomSound(SFX_POP, SFX_POP_NUM);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD2) || g_inputSystem->wasKeyJustPressed('2'))
	{
		blockType = STONE;
		g_theGame->PlayRandomSound(SFX_POP, SFX_POP_NUM);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD3) || g_inputSystem->wasKeyJustPressed('3'))
	{
		blockType = DIRT;
		g_theGame->PlayRandomSound(SFX_POP, SFX_POP_NUM);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD4) || g_inputSystem->wasKeyJustPressed('4'))
	{
		blockType = SAND;
		g_theGame->PlayRandomSound(SFX_POP, SFX_POP_NUM);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD5) || g_inputSystem->wasKeyJustPressed('5'))
	{
		blockType = WATER;
		g_theGame->PlayRandomSound(SFX_POP, SFX_POP_NUM);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD6) || g_inputSystem->wasKeyJustPressed('6'))
	{
		blockType = COBBLESTONE;
		g_theGame->PlayRandomSound(SFX_POP, SFX_POP_NUM);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_NUMPAD7) || g_inputSystem->wasKeyJustPressed('7'))
	{
		blockType = GLOWSTONE;
		g_theGame->PlayRandomSound(SFX_POP, SFX_POP_NUM);
	}
}

void GSClient::BlockPlaceRemoveInput()
{
	if (g_gameSession->my_player == nullptr || g_gameSession->my_player->mo_playerCharacter == nullptr)return;
	if (g_gameSession->my_player->mo_playerCharacter->m_myRaycast.didImpact == false) return;

	RaycastResult& ray = g_gameSession->my_player->mo_playerCharacter->m_myRaycast;
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_LMB) || g_inputSystem->wasKeyJustPressed('F'))
	{
		NetMessage msg(GAMEMSG_BLOCK_REMOVED);
		msg.sender = g_gameSession->m_session->my_connection;
		g_gameSession->m_session->SendMessageToOthers(msg);

		//Client side assume it was right
		g_theGame->m_world->digBlock(ray.blockInfo);
	}
	else if (g_inputSystem->wasKeyJustPressed(KEYCODE_RMB) || g_inputSystem->wasKeyJustPressed('G'))
	{
		NetMessage msg(GAMEMSG_BLOCK_PLACED);
		msg.sender = g_gameSession->m_session->my_connection;
		msg.write(g_gameSession->my_player->m_placementBlockType);

		g_gameSession->m_session->SendMessageToOthers(msg);

		//Client side assume it was right
		BlockInfo bi = ray.blockInfo;
		g_theGame->m_world->moveBlockInfoToFace(bi, ray.face);
		g_theGame->m_world->placeBlock(bi, g_gameSession->my_player->m_placementBlockType);
	}
}

//////////////////////////////////////////////////////////////////////////
void GSClient::drawClientInfo() const
{
	g_renderer->DrawText2D(Vector2(0, 88), "CLIENT", 2.f, Rgba::YELLOW, .75);

	g_renderer->translate2D(0.f, 86.f);
	drawPlayerInfoList();
}

void GSClient::drawPlayerInfoList() const
{
	std::string playerAmountTxt = Stringf("Players (%i/%i)", g_gameSession->m_players.size(), g_gameSession->m_session->m_maxConnectionCount);
	g_renderer->DrawText2D(Vector2::ZERO, playerAmountTxt, 2.f, Rgba::GRAY_192, .75);
	g_renderer->translate2D(0.f, -2.f);

	for (uint i = 0; i < g_gameSession->m_players.size(); ++i)
	{
		drawPlayerInfo(g_gameSession->m_players[i]);
		g_renderer->translate2D(0.f, -2.f);
	}
}

void GSClient::drawPlayerInfo(PlayerInfo* player) const
{
	if (player == nullptr)
		return;

	std::string strAddr = Stringf("- %i %s.", player->m_id, player->m_name.c_str());
	g_renderer->DrawText2D(Vector2::ZERO, strAddr, 2.f, Rgba::GRAY_192, .75);

	g_renderer->translate2D(0.f, -2.f);
	std::string inputStr = Stringf("  - Left: %i  Right: %i  Up: %i  Down: %i", player->m_left, player->m_right, player->m_up, player->m_down);
	g_renderer->DrawText2D(Vector2::ZERO, inputStr, 2.f, Rgba::GRAY_192, .75);
}

void GSClient::drawCrosshair() const
{
	g_renderer->SetOrthoProjection(-Vector2::ONE, Vector2::ONE);
	g_renderer->drawLineAndSetLineWidth(Vector2(0.01f, 0.f), Vector2(0.025f, 0.f), Rgba::WHITE, Rgba::WHITE, 5.f);
	g_renderer->drawLine(Vector2(-0.01f, 0.f), Vector2(-0.025f, 0.f), Rgba::WHITE);
	g_renderer->drawLine(Vector2(0.f, .01f), Vector2(0.f, .037f), Rgba::WHITE);
	g_renderer->drawLine(Vector2(0.f, -.01f), Vector2(0.f, -.037f), Rgba::WHITE);
}

void GSClient::drawPlayerBlockSelectUI(int selectedBlock) const
{
	float WINDOW_ASPECT_RATIO = static_cast<float>(g_windowWidth) / (0.5f * static_cast<float>(g_windowHeight));
	g_renderer->SetOrthoProjection(Vector2::ZERO, Vector2(100.f*WINDOW_ASPECT_RATIO, 100.f));
	g_renderer->BindTexture(&g_theGame->m_blockSpriteSheet->getTextureDX());
	Vector2 positionOffset(5.f, 5.f);
	const float scaleDownSquare = .9f;
	const float squareSize = 10.f;
	for (int blockTypeIndex = 1; blockTypeIndex < NUM_BLOCK_TYPES; blockTypeIndex++)
	{
		float scale = squareSize;
		Rgba color = Rgba::WHITE;
		if (blockTypeIndex != selectedBlock)
		{
			scale *= scaleDownSquare;
			color = Rgba::GRAY;
		}

		const BlockDefinition& blockDef = BlockDefinition::s_blockDefs[blockTypeIndex];
		const AABB2& blockPic = blockDef.m_textureCoords[EAST];

		g_renderer->drawTexturedSquare(positionOffset, scale, blockPic, color);

		positionOffset.x += squareSize;
	}
}
