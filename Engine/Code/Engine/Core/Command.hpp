#pragma once
#include "Engine/Core/EngineBase.hpp"
#include <string>
//////////////////////////////////////////////////////////////////////////
#pragma  warning(disable : 4100) // Disable "unreferenced formal parameter"

// 1. Predefine (locally) the method that embodies the commands work
// 2. Create a self register method that takes a pointer to the above function
// Start  the definition for the function, as the next thing the user types is the function body
#define COMMAND( name, helpText )	\
	static void AutoCommand_##name##(const std::string&);	\
	static CommandSelfRegister g_CommandRegister_##name##(#name, AutoCommand_##name##, helpText); \
	static void AutoCommand_##name##(const std::string& arguments)

// I actually use a helper object as the arguments variable
// so as to make parsing it easier in my actual engine - but for brevity
// we'll just pass the string past the command name
typedef void(*command_cb)(const std::string& arguments);

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/
class CommandSelfRegister
{
public:
	CommandSelfRegister(const std::string& id, command_cb cb, const char* helpText = nullptr);
};

bool CommandRegister(const std::string& id, command_cb cb, const char* helpText = nullptr);
bool CommandRun(const std::string& str);

const char* AutoCompleteCommandName(const std::string& str);