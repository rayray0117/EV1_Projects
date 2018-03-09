#include "Game/PlayerInfo.hpp"
#include "Engine/Networking/TCPSession.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameSession.hpp"
#include "Game/Actor.hpp"
#include "Engine/Core/Profiler.hpp"
//////////////////////////////////////////////////////////////////////////
PlayerInfo::PlayerInfo(Net::luid id, const std::string& name)
	: m_id(id)
	, m_name(name)
	, mo_playerCharacter(nullptr)
{
	/* DO NOTHING */
}

PlayerInfo::~PlayerInfo()
{
	/* DO NOTHING */
	SAFE_DELETE(mo_playerCharacter);
}

void PlayerInfo::Update(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	UNUSED(deltaSeconds);
	if (mo_playerCharacter == nullptr)
	{
		if(g_gameSession->isHost())
			g_gameSession->SpawnPlayer(this);

		return;
	}
	if (g_gameSession->isHost()) return;
	mo_playerCharacter->Update(deltaSeconds);
}

void PlayerInfo::Render() const
{
	PROFILE_SCOPE_FUNCTION();
	if (mo_playerCharacter == nullptr)
	{
		return;
	}

	mo_playerCharacter->Render();
}

//////////////////////////////////////////////////////////////////////////