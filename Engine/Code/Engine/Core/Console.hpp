#pragma once
#include <string>
#include <map>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RenderableString.hpp"
#include "Engine/Core/EngineBase.hpp"
#include <vector>
#include "Engine/Core/Singleton.hpp"
#include "Event.hpp"
// This does scary Macro stuff and I don't completely know what I'm doing but let's have fun anyway!
//////////////////////////////////////////////////////////////////////////
// #define DECLARE_COMMAND(name) static void AutoCommand_##name##(Command &args)
// 
// #define COMMAND(name, syntax, help_text) DECLARE_COMMAND(name);
// 	static CommandRegistration g_AutoCommandRegistration_##name##(#name, AutoCommand_##name##) 
//	DECLARE_COMMAND(name)
class SimpleRenderer;
class Renderer;
class SpriteAnim;
//////////////////////////////////////////////////////////////////////////
typedef bool(*ConsoleCommand)(std::string args);
const float DEFAULT_CONSOLE_TEXT_SCALE = 20.f;

//////////////////////////////////////////////////////////////////////////
class Console : public Singleton<Console>
{
	friend Singleton;
	Console();
	~Console();
public:

	const std::vector<RenderableString>&getMessages() const;
	void Render(SimpleRenderer* renderer) const;
	void Render(Renderer* renderer) const;

	void addMessage(const RenderableString& msg);
	void addMessage(const std::string& msg);
	void addMessage(const std::string& msg, float scale, const Rgba& color);
	void addMessage(const std::string& msg, const Rgba& color);

	void OnKeyPressed(unsigned char keyCode);
	void OnKeyCodePressed(unsigned char keyCode);

public:
	std::map<std::string, ConsoleCommand> m_commands;
	std::vector<RenderableString> m_msgs;
	std::vector<std::string> m_typedCommands; //Lists of previous typed in commands. Helps it so I'm not putting in plain messages when trying to use a previous called command or mispelled something.
	std::string m_currentBuffer = "";
	int m_msgsPos;
	std::string::const_iterator m_caretPos = m_currentBuffer.begin();

	float m_textSize;

	bool m_haveConsolePrepareRendering;
	bool m_useBitmap = true;

	SpriteAnim* m_anim;

	static Event<const std::string&> OnMessagePrint;
private:
	void drawMsgs_Font(SimpleRenderer* renderer) const;
	void drawMsgs_BitmapFont(SimpleRenderer* renderer) const;
};

#define CONSOLE Console::GetInstance()