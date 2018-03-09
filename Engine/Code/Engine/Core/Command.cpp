#include "Engine/Core/Command.hpp"
#include <map>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
//////////////////////////////////////////////////////////////////////////

class Command
{
public:
	Command(const std::string& name, command_cb cb, const std::string& helpText)
		: m_name(name)
		, m_callback(cb)
		, m_helpText(helpText)
	{}
	
	std::string m_name;
	command_cb m_callback;
	std::string m_helpText;
};

//////////////////////////////////////////////////////////////////////////

class CommandSystem
{
public:
	CommandSystem() {}
	~CommandSystem()
	{
		for (auto &iter : m_commandLookup)
		{
			delete iter.second;
		}

		m_commandLookup.clear();
	}

	bool add_command(const std::string& name, command_cb cb, const std::string& help_text)
	{
		if (find_command(name) != nullptr) 
		{
			// ASSERT OR DIE - DUPLICATE
			return false;
		}

		Command* command = new Command(name, cb, help_text);
		m_commandLookup.insert(std::pair<std::string, Command*>(name, command));
		return true;
	}

	//------------------------------------------------------------------------
	Command* find_command(const std::string& name) const
	{
		auto iter = m_commandLookup.find(name);
		if (iter != m_commandLookup.end()) 
		{
			return iter->second;
		}
		else {
			return nullptr;
		}
	}

public:
	std::map<std::string, Command*> m_commandLookup;
};
////////////////////////////////////////////////////////////////////////// //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static CommandSystem *g_CommandSystem = nullptr;

//------------------------------------------------------------------------
CommandSelfRegister::CommandSelfRegister(const std::string& id, command_cb cb, const char* help_text /*= nullptr*/)
{
	CommandRegister(id, cb, help_text);
}

//------------------------------------------------------------------------
bool CommandRegister(const std::string& id, command_cb cb, const char* help_text /*= nullptr*/)
{
	// first caller will instantiate
	if (g_CommandSystem == nullptr) {
		g_CommandSystem = new CommandSystem();
	}

	return g_CommandSystem->add_command(id, cb, help_text);
}

//------------------------------------------------------------------------
bool CommandRun(const std::string& str)
{
	if (g_CommandSystem == nullptr) {
		return false;
	}

	// Extract the command part from the arguments part.
	std::string line = str;
	size_t first_space = line.find(' ');
	std::string cmd = line.substr(0, first_space);
	std::string args;
	if (first_space != std::string::npos) 
	{
		args = line.substr(first_space + 1U);
	}

	// Find command.
	Command* cmdreg = g_CommandSystem->find_command(cmd.c_str());
	if (nullptr == cmdreg) 
	{
		RenderableString temp("ERROR: " + str + " is invalid!", 1.25f, Rgba::RED);
		g_theConsole->addMessage(temp);
		return false;
	}

	// if we have it, run it with supplied arguments
	RenderableString temp(str, 1.f, Rgba::WHITE);
	g_theConsole->addMessage(temp);
	g_theConsole->m_typedCommands.push_back(str);
	cmdreg->m_callback(args.c_str());
	return true;
}


const char* AutoCompleteCommandName(const std::string& str)
{
	if (str.empty())
		return nullptr;

	for (auto& currentCommand : g_CommandSystem->m_commandLookup)
	{
		if (currentCommand.first.find(str) == 0)
		{
			return currentCommand.first.c_str();
		}
	}

	return nullptr;
}

/************************************************************************/
/*                                                                      */
/* COMMANDS                                                             */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
COMMAND(help, "Displays all commands.")
{
	if (g_CommandSystem == nullptr) {
		return; // no commands?  how did this get called?!
	}

	for (auto const &iter : g_CommandSystem->m_commandLookup) {
		Command *cmd = iter.second;

		// should print to console
		RenderableString msg;
		msg.m_string = Stringf("%s: %s", cmd->m_name.c_str(), cmd->m_helpText.c_str());
		msg.m_color = Rgba::GRAY_192;
		g_theConsole->addMessage(msg.m_string, msg.m_color);
	}
}