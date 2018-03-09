#pragma once
#include "Engine\Networking\Net.hpp"
#include "Game\GameCommon.hpp"
//////////////////////////////////////////////////////////////////////////
class Ship;
class Actor;
//////////////////////////////////////////////////////////////////////////
class PlayerInfo
{
public:
	PlayerInfo(Net::luid id, const std::string& name);
	~PlayerInfo();

	void Update(float deltaSeconds);
	void Render() const;
public:
	Net::luid m_id;
	std::string m_name;
	Actor* mo_playerCharacter = nullptr;
	int m_left = 0;
	int m_right = 0;
	int m_up = 0;
	int m_down = 0;
	float m_clientsJoinedTime;
	float m_hostRecievedTime;
	BlockTypes m_placementBlockType = COBBLESTONE;
};