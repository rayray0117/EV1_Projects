#include "Game/GameSession.hpp"
#include "Engine/Networking/TCPSession.hpp"
#include "Engine/Networking/NetMessage.hpp"
#include "PlayerInfo.hpp"
#include "Engine/Networking/NetConnection.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/World.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "GSJoin.hpp"
#include "Game/Actor.hpp"
#include "Game/GSHost.hpp"
#include "Engine/Networking/TCPConnection.hpp"
#include "Engine/Core/Profiler.hpp"
//////////////////////////////////////////////////////////////////////////
GameSession::GameSession()
	:my_player(nullptr)
{
	m_session = new TCPSession();
	NetObjectSystem::StartUp(m_session, NET_UPDATE_HZ);
	SetupMessageDefinitions();
	SetupNetObjectDefinitions();
	NetObjectSystem::g_auth = SEVER_CLINET;
}

GameSession::~GameSession()
{
	SAFE_DELETE(m_session);
}
//////////////////////////////////////////////////////////////////////////
void GameSession::Update(float deltaSeconds)
{
	if (m_session->isRunning())
	{
		m_session->Update();
		UpdatePlayers(deltaSeconds);
	}
}

void GameSession::Host(Net::portId port)
{
	m_session->Host(port);
	m_session->StartListening();
}

bool GameSession::Join(const std::string& address)
{
	m_session->Leave();
	return m_session->Join(StringToNetAddress(address));
}

void GameSession::SetupMessageDefinitions()
{
	m_session->register_message_function(GAMEMSG_JOIN_REQUEST, [this](NetMessage* msg) {this->on_join_request(msg); });
	m_session->register_message_function(GAMEMSG_JOIN_RESPONSE, [this](NetMessage* msg) {this->on_join_response(msg); });
	m_session->register_message_function(GAMEMSG_INPUT_UPDATE, [this](NetMessage* msg) {this->on_input_update(msg); });
	m_session->register_message_function(GAMEMSG_PLAYER_UPDATE, [this](NetMessage* msg) {this->on_player_update(msg); });
	m_session->register_message_function(GAMEMSG_CAMERA_UPDATE, [this](NetMessage* msg) {this->on_camera_update(msg); });
	m_session->register_message_function(GAMEMSG_CONNECTION_LOST, [this](NetMessage* msg) {this->on_connection_lost(msg); });
	m_session->register_message_function(GAMEMSG_BLOCK_REMOVED, [this](NetMessage* msg) {this->on_block_removed(msg); });
	m_session->register_message_function(GAMEMSG_BLOCK_PLACED, [this](NetMessage* msg) {this->on_block_placed(msg); });
	m_session->register_message_function(GAMEMSG_RLE_FORCE_FIX, [this](NetMessage* msg) {this->on_rle_forced(msg); });
	m_session->register_message_function(GAMEMSG_RLE_GUESS, [this](NetMessage* msg) {this->on_rle_guess_received(msg); });
}

void GameSession::SetupNetObjectDefinitions()
{
	nos::NetObjectTypeDefinition* shipDef = new nos::NetObjectTypeDefinition();
	shipDef->append_create_info = Actor::AppendCreateInfo;
	shipDef->process_create_info = Actor::ProcessCreateInfo;
	shipDef->append_destroy_info = nullptr;
	shipDef->process_destroy_info = Actor::ProcessDestroyInfo;
	shipDef->append_snapshot = Actor::AppendSnap;
	shipDef->process_snapshot = Actor::ProcessSnap;
	shipDef->apply_snapshot = Actor::ApplySnap;
	shipDef->get_current_snapshot = Actor::GetCurrentSnap;
	nos::AddNetObjectDefinition(NETOBJ_PLAYER_CHARACTER, shipDef);
}

void GameSession::on_join_request(NetMessage* msg)
{
	float joinResquestReceivedAtTime = (float)GetCurrentTimeSeconds();
	std::string command_and_args;// = 
	msg->read_string(command_and_args);
	std::vector<std::string> args;
	splitArgs(command_and_args, args);
	
	PlayerInfo* newPlayer = new PlayerInfo(msg->sender->connection_index, args[1]);
	newPlayer->m_hostRecievedTime = joinResquestReceivedAtTime;
	newPlayer->m_clientsJoinedTime = std::stof(args[0]);
	m_players.push_back(newPlayer);

	NetMessage join_response(GAMEMSG_JOIN_RESPONSE);
	join_response.sender = m_session->my_connection;
	std::string responseStr = Stringf("%i %i %i", msg->sender->connection_index, g_Seed, m_players.size());
	for (uint i = 0; i < m_players.size(); ++i)
	{
		if(m_players[i] == nullptr)
			continue;

		std::string playerInfoStr = Stringf(" %i %s", m_players[i]->m_id, m_players[i]->m_name.c_str());
		responseStr += playerInfoStr;
	}
	std::string timeRefStr = Stringf(" %f", (float)GetCurrentTimeSeconds());
	responseStr += timeRefStr;

	join_response.write_string(responseStr);
	g_gameSession->m_session->SendMessageToIndex(msg->sender->connection_index, join_response);
	SendPlayerUpdateMsg(newPlayer);
	nos::SendCreateInfosForJoinRequest(m_session->getConnection(msg->sender->connection_index));
}

void GameSession::on_join_response(NetMessage* msg)
{
	m_localTimeReceivedJoinResponse = (float)GetCurrentTimeSeconds();
	std::string command_and_args;// = 
	msg->read_string(command_and_args);

	std::vector<std::string> args;
	splitArgs(command_and_args, args);

	Net::luid id = (uint8)std::stoi(args[0]);
	int worldSeed = std::stoi(args[1]);

	uint numPlayers = (uint)std::stoi(args[2]);
	uint index = 3;
	for (uint i = 0; i < numPlayers; ++i)
	{
		Net::luid pid = (uint8)std::stoi(args[index]);
		std::string pname = args[index+1];

		m_players.push_back(new PlayerInfo(pid, pname));
		index += 2;
	}
	m_joinAtHostTime = std::stof(args[index]);

	GSJoin* joinState = (GSJoin*)g_theGame->m_currentGameState;
	joinState->FinishJoin(id, worldSeed);
}

void GameSession::on_input_update(NetMessage* msg)
{
	std::string msgStr;// = 
	msg->read_string(msgStr);

	std::vector<std::string> args;
	splitArgs(msgStr, args);

	PlayerInfo* player = getPlayerInfo(msg->sender->connection_index);
	for (uint i = 0; i < args.size(); i+=2)
	{
		if (args[i] == "l")
		{
			int t = std::stoi(args[i+1]);
			player->m_left = t;
		}
		else if (args[i] == "r")
		{
			int t = std::stoi(args[i+1]);
			player->m_right = t == 1;
		}
		else if (args[i] == "u")
		{
			int t = std::stoi(args[i+1]);
			player->m_up = t;
		}
		else if (args[i] == "d")
		{
			int t = std::stoi(args[i+1]);
			player->m_down = t;
		}
	}
	
	SendPlayerUpdateMsg(player);
}

void GameSession::SendPlayerUpdateMsg(PlayerInfo* player)
{
	NetMessage updateToSend(GAMEMSG_PLAYER_UPDATE);
	updateToSend.sender = m_session->my_connection;
	std::string playerInfoStr = Stringf("%i %s %i %i %i %i", player->m_id, player->m_name.c_str(), player->m_left, player->m_right, player->m_up, player->m_down);
	updateToSend.write_string(playerInfoStr);
	m_session->SendMessageToOthers(updateToSend);
}

void GameSession::on_player_update(NetMessage* msg)
{
	std::string msgStr;
	msg->read_string(msgStr);

	std::vector<std::string> args;
	splitArgs(msgStr, args);

	Net::luid id = (uint8)std::stoi(args[0]);
	std::string name = args[1];
	int left = std::stoi(args[2]);
	int right = std::stoi(args[3]);
	int up = std::stoi(args[4]);
	int down = std::stoi(args[5]);

	PlayerInfo* player = getPlayerInfo(id);
	if (player != nullptr)
	{
		player->m_name = name;
		player->m_left = left;
		player->m_right = right;
		player->m_up = up;
		player->m_down = down;
	}
	else if (player == nullptr)
	{
		player = new PlayerInfo(id, name);
		player->m_left = left;
		player->m_right = right;
		player->m_up = up;
		player->m_down = down;
		m_players.push_back(player);
	}
}

void GameSession::on_camera_update(NetMessage* msg)
{
	PlayerInfo* playInfo = getPlayerInfo(msg->sender->connection_index);
	float yaw, pitch;
	msg->read(yaw);
	msg->read(pitch);
	if (playInfo->mo_playerCharacter != nullptr)
	{
		playInfo->mo_playerCharacter->m_yawDegreesAboutZ = yaw;
		playInfo->mo_playerCharacter->m_pitchDegreesAboutY = pitch;
	}
}

void GameSession::on_connection_lost(NetMessage* msg)
{
	Net::luid id;
	msg->read(id);

	for (uint index = 0; index < m_players.size(); index++)
	{
		if (m_players[index]->m_id == id)
		{
			SAFE_DELETE(m_players[index]);
		}
	}

	CleanUpPlayInfos();
}
//////////////////////////////////////////////////////////////////////////
void GameSession::on_block_removed(NetMessage* msg)
{
	PlayerInfo* playInfo = getPlayerInfo(msg->sender->connection_index);

	if (playInfo == nullptr || playInfo->mo_playerCharacter == nullptr) return;

	RaycastResult& ray = playInfo->mo_playerCharacter->m_myRaycast;
	if (ray.blockInfo.m_chunk == nullptr)
	{
		//If the player is somehow in an area that is not activated on the host. Activate it.
		g_theGame->m_world->ActiveClosestChunkToPos(playInfo->mo_playerCharacter->m_position);
		playInfo->mo_playerCharacter->RaycastCheck();
	}
	if (ray.didImpact)
	{
		g_theGame->m_world->digBlock(ray.blockInfo);
		
		//Force an update to everyone
		std::vector<uchar> rleData;
		IntVector2 coords = ray.blockInfo.m_chunk->m_chunkCoords;
		auto found = g_theGame->m_world->m_rleChunks.find(coords);
		if (found == g_theGame->m_world->m_rleChunks.end())
		{
			ray.blockInfo.m_chunk->m_rle;
			g_theGame->m_world->m_rleChunks[coords] = rleData;
		}
		else
		{
			rleData = found->second;
		}

		

		NetMessage response(GAMEMSG_RLE_FORCE_FIX);
		response.sender = m_session->my_connection;
		response.write(coords);
		writeRLE(response, rleData);
		m_session->SendMessageToOthers(response);
	}
}

void GameSession::on_block_placed(NetMessage* msg)
{
	PlayerInfo* playInfo = getPlayerInfo(msg->sender->connection_index);

	if (playInfo == nullptr || playInfo->mo_playerCharacter == nullptr) return;

	BlockTypes blockType;
	msg->read(blockType);

	RaycastResult& ray = playInfo->mo_playerCharacter->m_myRaycast;
	if (ray.blockInfo.m_chunk == nullptr)
	{
		//If the player is somehow in an area that is not activated on the host. Activate it.
		g_theGame->m_world->ActiveClosestChunkToPos(playInfo->mo_playerCharacter->m_position);
		playInfo->mo_playerCharacter->RaycastCheck();
	}
	if (ray.didImpact)
	{
		BlockInfo bi = ray.blockInfo;
		g_theGame->m_world->moveBlockInfoToFace(bi, ray.face);
		g_theGame->m_world->placeBlock(bi, blockType);


		//Force an update to everyone
		std::vector<uchar> rleData;
		IntVector2 coords = ray.blockInfo.m_chunk->m_chunkCoords;
		auto found = g_theGame->m_world->m_rleChunks.find(coords);
		if (found == g_theGame->m_world->m_rleChunks.end())
		{
			ray.blockInfo.m_chunk->m_rle;
			g_theGame->m_world->m_rleChunks[coords] = rleData;
		}
		else
		{
			rleData = found->second;
		}



		NetMessage response(GAMEMSG_RLE_FORCE_FIX);
		response.sender = m_session->my_connection;
		response.write(coords);
		writeRLE(response, rleData);
		m_session->SendMessageToOthers(response);
	}
}

void GameSession::on_rle_forced(NetMessage* msg)
{
	IntVector2 coords;
	msg->read(coords);
	std::vector<uchar> rleData;
	readRLE(*msg, rleData);

	g_theGame->m_world->SetChunkRLE(coords, rleData);
}

void GameSession::on_rle_guess_received(NetMessage* msg)
{
	IntVector2 coords;
	msg->read(coords);
	std::vector<uchar> rleData;
	readRLE(*msg, rleData);

	if (g_theGame->m_world->isChunkRLECorrect(coords, rleData)) return;

	auto found = g_theGame->m_world->m_rleChunks.find(coords);
	rleData = found->second;

	NetMessage response(GAMEMSG_RLE_FORCE_FIX);
	response.sender = m_session->my_connection;
	response.write(coords);
	writeRLE(response, rleData);
	m_session->SendMessageToIndex(msg->sender->connection_index, response);
}
//////////////////////////////////////////////////////////////////////////
void GameSession::UpdatePlayers(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	for (uint i = 0; i < m_players.size(); i++)
	{
		if (m_players[i] != nullptr)
		{
			TCPConnection* connection = (TCPConnection*)m_session->getConnection(m_players[i]->m_id);
			if (isHost() && ( connection == nullptr || connection->isDisconnected()))
			{
				NetMessage msg(GAMEMSG_CONNECTION_LOST);
				msg.write(m_players[i]->m_id);
				msg.sender = m_session->my_connection;
				m_session->SendMessageToOthers(msg);

				SAFE_DELETE(m_players[i]);
			}
			else if (m_players[i] != nullptr)
			{
				m_players[i]->Update(deltaSeconds);
			}
		}
	}

	CleanUpPlayInfos();

}

void GameSession::CleanUpPlayInfos()
{
	for (uint index = 0; index < m_players.size(); index++)
	{
		if (nullptr == m_players[index])
		{
			m_players.erase(m_players.begin() + index);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void GameSession::writeRLE(BinaryStream &stream, const std::vector<uchar>& buffer)
{
	stream.write(buffer.size()); //write the amount of uchars to be sent.
	for (size_t i = 0; i < buffer.size(); ++i)
	{
		stream.write(buffer[i]);
	}
}

void GameSession::readRLE(BinaryStream &stream, std::vector<uchar>& buffer)
{
	size_t amount;
	stream.read(amount);
	buffer.resize(amount);
	for (size_t i = 0; i < amount; ++i)
	{
		stream.read(buffer[i]);
	}
}
//////////////////////////////////////////////////////////////////////////
PlayerInfo* GameSession::getPlayerInfo(Net::luid id)
{
	for (PlayerInfo* var : m_players)
	{
		if(var == nullptr)
			continue;

		if (var->m_id == id)
			return var;
	}

	return nullptr;
}

Actor* GameSession::getMyPlayer()
{
	if (my_player == nullptr)
		return nullptr;
	return my_player->mo_playerCharacter;
}

Actor* GameSession::SpawnPlayer(PlayerInfo* caller)
{
	if (!m_session->isHost())
		return nullptr;

	Actor* playerChar = new Actor(Vector3(0.f, 0.f, 100.5f), caller->m_id);
	//g_theGame->m_world->m_actors.push_back(playerChar);
	nos::NetObject* obj = NetObjectSystem::Replicate(playerChar, NETOBJ_PLAYER_CHARACTER);
	playerChar->m_noId = obj->net_id;
	ActorSync* sync = new ActorSync();
	/*/
	sync->position = playerChar->m_position;
	sync->pitchDegreesAboutY = playerChar->m_pitchDegreesAboutY;
	sync->yawDegreesAboutZ = playerChar->m_yawDegreesAboutZ;
	//*/
	obj->current_snapshot = sync;
	obj->last_received_snapshot = new ActorSync();

	return playerChar;
}

//////////////////////////////////////////////////////////////////////////
const bool GameSession::isHost() const
{
	return m_session->isHost();
}

Camera3D* GameSession::getCamera() const
{
	if (isHost())
	{
		GSHost* host = (GSHost*)g_theGame->m_currentGameState;
		return host->m_camera;
	}
	if (my_player == nullptr || my_player->mo_playerCharacter == nullptr)
		return nullptr;
		
	return my_player->mo_playerCharacter->m_camera;
}
