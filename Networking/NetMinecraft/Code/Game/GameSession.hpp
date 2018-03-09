//Just a wrapper for me to use to make handling Game Session easier
#pragma once
#include <vector>
#include "Engine\Networking\Net.hpp"
#include "Engine\Networking\NetObjectSystem.hpp"
//////////////////////////////////////////////////////////////////////////
class TCPSession;
class PlayerInfo;
class NetMessage;
namespace nos = NetObjectSystem;
class nos::NetObject;
class Ship;
class Actor;
class Camera3D;
class BinaryStream;
//////////////////////////////////////////////////////////////////////////
enum EGameMessage
{
	GAMEMSG_JOIN_REQUEST = NUM_NETOBJECT_MGS,
	GAMEMSG_JOIN_RESPONSE,
	GAMEMSG_INPUT_UPDATE,
	GAMEMSG_PLAYER_UPDATE,
	GAMEMSG_CAMERA_UPDATE,
	GAMEMSG_CONNECTION_LOST,
	GAMEMSG_BLOCK_REMOVED,
	GAMEMSG_BLOCK_PLACED,
	GAMEMSG_RLE_FORCE_FIX,
	GAMEMSG_RLE_GUESS,
	NUM_GAME_MGS
};
enum ENetObjectType
{
	NETOBJ_PLAYER_CHARACTER,
	NUM_NETOBJECT_TYPES
};
//////////////////////////////////////////////////////////////////////////
class GameSession
{
public:
	GameSession();
	~GameSession();
	
	void Update(float deltaSeconds);
	void Host(Net::portId port);
	bool Join(const std::string& address);

	void SetupMessageDefinitions();
	void SetupNetObjectDefinitions();
	PlayerInfo* getPlayerInfo(Net::luid id);
	Actor* getMyPlayer();
	Actor* SpawnPlayer(PlayerInfo* caller);

	const bool isHost() const;
	Camera3D* getCamera() const;
public:
	TCPSession* m_session;
	std::vector<PlayerInfo*> m_players;
	PlayerInfo* my_player = nullptr;
	float m_localTimeReceivedJoinResponse;
	float m_joinAtHostTime;
private:
	void on_join_request(NetMessage* msg);
	void on_join_response(NetMessage* msg);
	void on_input_update(NetMessage* msg);

	void SendPlayerUpdateMsg(PlayerInfo* player);

	void on_player_update(NetMessage* msg);
	void on_camera_update(NetMessage* msg);
	void on_connection_lost(NetMessage* msg);
	void on_block_removed(NetMessage* msg);
	void on_block_placed(NetMessage* msg);
	void on_rle_forced(NetMessage* msg);
	void on_rle_guess_received(NetMessage* msg);

	void UpdatePlayers(float deltaSeconds);

	void CleanUpPlayInfos();

	//Host functions
public:
	static void writeRLE(BinaryStream &stream,const std::vector<uchar>& buffer);
	static void readRLE(BinaryStream &stream, std::vector<uchar>& buffer);
};