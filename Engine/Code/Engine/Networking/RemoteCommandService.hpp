#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Singleton.hpp"
#include "NetMessage.hpp"
#include "TCPSession.hpp"
#include "NetConnection.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Networking/NetMessageDefinition.hpp"
#include "../Core/StringUtils.hpp"
#include "../Core/Console.hpp"
//////////////////////////////////////////////////////////////////////////
class RemoteCommandService : public Singleton<RemoteCommandService>
{
public:

	void Update()
	{
		if (session.isRunning())
		{
			session.Update();
		}
		else
		{
			net_address join_addr = GetMyAddress(RCS_PORT);
			//ConfigGetAddress(&join_addr);
			if (!session.Join(GetMyAddress(RCS_PORT)))
			{
				session.Host(RCS_PORT);
				session.StartListening();
			}
		}
	}


	void Join(net_address const &addr)
	{
		session.Leave();
		session.Join(addr);
	}

	void setup_message_definition()
	{
		//send/recv commands
		//send message
		//NetMessageDefinition* msgDefCommand;
		//msgDefCommand->handler.subscribe(on_command)
		session.register_message_function(RCS_COMMAND, [this](NetMessage* msg) {this->on_command(msg); });
		session.register_message_function(RCS_ECHO, [this](NetMessage* msg) {this->on_message(msg); });
	}

	void on_command(NetMessage* msg)
	{
		std::string command_and_args;// = 
		msg->read_string(command_and_args);
		//TODO: Print someone sent me a command - addr: command_and_args

		current_sender = msg->sender;
		Console::OnMessagePrint.subscribe_method(this, &RemoteCommandService::send_message);
		CommandRun(command_and_args);
		Console::OnMessagePrint.unsubscribe_method(this, &RemoteCommandService::send_message);
		current_sender = nullptr;
	}

	void on_message(NetMessage* msg)
	{
		std::string response; // =
		msg->read_string(response);
		//ConsolePrintf("[%s] %s", )
		Console::GetInstance()->addMessage(Stringf("[%s] %s", NetAddressToString(msg->sender->address).c_str(), response.c_str()));
	}

	void send_message(const std::string& str)
	{
		NetMessage* msg = new NetMessage(RCS_ECHO);
		msg->write_string(str);
		current_sender->Send(msg);
	}

	void send_command_to_others(const char* command_and_args)
	{
		// make a message
		NetMessage message(RCS_COMMAND);

		// fill message
		message.write_string(command_and_args);
		// send message to everyone but myself
		session.SendMessageToOthers(message);
	}

	void send_command_to_all(const char* command_and_args)
	{
		// make a message
		NetMessage message(RCS_COMMAND);

		// fill message
		message.write_string(command_and_args);
		// send message to everyone but myself
		session.SendMessageToAll(message);
	}
public:
	TCPSession session;

	NetConnection* current_sender;


};
//////////////////////////////////////////////////////////////////////////
#define RCS RemoteCommandService::GetInstance()
//////////////////////////////////////////////////////////////////////////
COMMAND(rc, "run a console cmd on a remote console")
{
	RemoteCommandService *rcs = RemoteCommandService::GetInstance();

	const char* command_and_args = arguments.c_str();
	rcs->send_command_to_others(command_and_args);
}

COMMAND(rcs_join, "")
{
	net_address addr = StringToNetAddress(arguments);
	RemoteCommandService::GetInstance()->Join(addr);
}

COMMAND(rca, "send a command to everyone including me!")
{
	const char* command_and_args = arguments.c_str();
	RCS->send_command_to_all(command_and_args);
}

COMMAND(rc_idx, "send a command to a specific indexed address")
{
	NetMessage message(RCS_COMMAND);

	std::string indexStr, argStr;
	size_t space = arguments.find(" ");
	indexStr = arguments.substr(0, space);
	argStr = arguments.substr(space + 1U);
	uint8 index = (uint8)std::stoi(indexStr);
	message.write_string(argStr);

	RCS->session.SendMessageToIndex(index, message);
}