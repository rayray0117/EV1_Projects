#include "Game/GSJoin.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Game/GameSession.hpp"
#include "Game/World.hpp"
#include "Engine/Networking/NetMessage.hpp"
#include "Engine/Networking/TCPSession.hpp"
#include "PlayerInfo.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////
const uint MAX_OPTIONS = 4;
//////////////////////////////////////////////////////////////////////////
GSJoin::GSJoin()
{
	m_state = GAMESTATE_JOIN;

	m_hostAddress = NetAddressToString(GetMyAddress(28210));
	m_username = "bland";
}

GSJoin::~GSJoin()
{

}
//////////////////////////////////////////////////////////////////////////
void GSJoin::Update(float deltaSeconds)
{
	HandleInput();

	if(g_gameSession != nullptr)
		g_gameSession->Update(deltaSeconds);
}

void GSJoin::Render() const
{
	drawMenuItems();
}
//////////////////////////////////////////////////////////////////////////
void GSJoin::HandleInput()
{
	if (g_inputSystem->wasKeyJustPressed(KEYCODE_ESCAPE))
	{
		g_theApp->m_isQuitting = true;
	}
	else
	if (wasInputJustPressed(INPUT_UP))
	{
		if (m_optionIndex == 0)
			return;

		--m_optionIndex;
	}
	else
	if (wasInputJustPressed(INPUT_DOWN))
	{
		if (m_optionIndex == MAX_OPTIONS-1)
			return;

		++m_optionIndex;
	}
	else
	if (wasInputJustPressed(INPUT_ACCEPT))
	{
		CallOption();
	}
}

void GSJoin::FinishJoin(Net::luid id, int worldSize)
{
	g_gameSession->my_player = g_gameSession->getPlayerInfo(id);
	ASSERT_OR_DIE(g_gameSession->my_player != nullptr, "Could not find my player info in list!");

	g_Seed = worldSize;

	g_theGame->ChangeGameState(GAMESTATE_CLIENT);
}

void GSJoin::CallOption()
{
	switch (m_optionIndex)
	{
	case 0: CallSetName();
		break;
	case 1: CallSetHost();
		break;
	case 2: TryToJoin();
		break;
	case 3: g_theGame->ChangeGameState(GAMESTATE_MAINMENU);
		break;
	default:
		break;
	}
}

void GSJoin::TryToJoin()
{
	if (m_username.empty() || m_hostAddress.empty())
		return;

	BeginJoin();
}

void GSJoin::BeginJoin()
{
	g_gameSession = new GameSession();

	if (!g_gameSession->Join(m_hostAddress))
		return;
		
	NetMessage message(GAMEMSG_JOIN_REQUEST);
	message.sender = g_gameSession->m_session->my_connection;
	message.write_string(Stringf("%f ", (float)GetCurrentTimeSeconds()) + m_username);
	g_gameSession->m_session->SendMessageToOthers(message);
}

void GSJoin::drawMenuItems() const
{
	BitmapFont* font = g_renderer->CreateOrGetBitmapFont(DEFAULT_FONT);
	g_renderer->ClearScreen(Rgba::SKY_MOONLIGHT);
	g_renderer->SetOrthoProjection(IntVector2::ZERO, IntVector2(16, 9));
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 7.f), "JOIN", 1.5f, Rgba::WHITE, 1.f, font);


	std::string usernameTxt = "Username: " + m_username;
	std::string hostaddrTxt = "Host Address: " + m_hostAddress;
	
	//Option buttons
	Rgba nameColor = m_optionIndex == 0 ? Rgba::YELLOW : Rgba::WHITE;
	Rgba hostColor = m_optionIndex == 1 ? Rgba::YELLOW : Rgba::WHITE;
	Rgba joinColor = m_optionIndex == 2 ? Rgba::YELLOW : Rgba::WHITE;
	Rgba backColor = m_optionIndex == 3 ? Rgba::YELLOW : Rgba::WHITE;
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 5.5f), usernameTxt, .5f, nameColor, .5f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 5.0f), hostaddrTxt, .5f, hostColor, .5f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 3.5f), "Join", .75f, joinColor, .5f, font);
	g_renderer->drawText2DWithBitmapFont_Centered(Vector2(8.f, 2.5f), "Back", .75f, backColor, .5f, font);
}

void GSJoin::CallSetName()
{
	g_theConsole->addMessage("Type in your username, then press enter!", 2.f, Rgba::GREEN);
	g_theConsole->m_currentBuffer = "game_username ";
	g_consoleVisable = true;
}

void GSJoin::CallSetHost()
{
	g_theConsole->addMessage("Type in the host address and port, then press enter!", 2.f, Rgba::GREEN);
	g_theConsole->m_currentBuffer = "game_host_address ";
	g_consoleVisable = true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(game_host_address, "Params: net address:port. Change the host address to join")
{
	if (g_theGame->m_currentGameState->m_state != GAMESTATE_JOIN)
	{
		ERROR_RECOVERABLE("Tried to set host address while not on the join menu!");
		return;
	}

	GSJoin* joinState = (GSJoin*)g_theGame->m_currentGameState;
	joinState->m_hostAddress = arguments;
}

COMMAND(game_username, "Params: string. Changes username")
{
	//Make sure you are in the join state for now.
	if (g_theGame->m_currentGameState->m_state != GAMESTATE_JOIN)
	{
		ERROR_RECOVERABLE("Tried to set username while not on the join menu!");
		return;
	}

	GSJoin* joinState = (GSJoin*)g_theGame->m_currentGameState;
	joinState->m_username = arguments;
}