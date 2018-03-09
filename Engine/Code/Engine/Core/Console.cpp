#include "Engine/Core/Console.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include <algorithm>
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Font.hpp"
#include "Engine/Input/InputSystem.hpp"
#include <ctype.h>
#include "Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
//////////////////////////////////////////////////////////////////////////
const std::string CONSOLE_FONT_PATH = "Data/Fonts/Garamound32.fnt";
const std::string CONSOLE_BITMAP_FONT_NAME = "SquirrelFixedFont";
Event<const std::string&> Console::OnMessagePrint;
//////////////////////////////////////////////////////////////////////////
Console::Console()
	: m_msgsPos(0)
	, m_textSize(DEFAULT_CONSOLE_TEXT_SCALE)
	, m_haveConsolePrepareRendering(false)
	, m_anim(nullptr)
{
}

Console::~Console()
{
	SAFE_DELETE(m_anim);
}

//////////////////////////////////////////////////////////////////////////
const std::vector<RenderableString>& Console::getMessages() const
{
	return m_msgs;
}

void Console::Render(SimpleRenderer* renderer) const
{
	if (m_haveConsolePrepareRendering)
	{
		renderer->SetBlendFunc(RHIEnum::BLEND_SOURCE_ALPHA, RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA);
		renderer->EnableDepth(false);
		renderer->BindShader(DEFAULT_UNLIT_SHADER);
		renderer->SetOrthoProjection(Vector2::ZERO, Vector2::ONE);
		renderer->drawAABB2(AABB2::ZERO_TO_ONE, Rgba(Rgba::BLACK, .5));
		if(m_anim != nullptr)
			renderer->drawTexturedAABB2(AABB2(Vector2(.75, .75), Vector2::ONE), m_anim->GetTexture2D(),m_anim->GetCurrentTexCoords());
		renderer->SetOrthoProjection(Vector2::ZERO, Vector2(1280, 720));
	}
	Vector2 bottomLeft(0.f, 0.f);
	int time = (int)GetCurrentTimeSeconds();
	/*/
	if (time % 2 == 0)
	{
		renderer->drawText2D(bottomLeft, m_currentBuffer + ">", m_textSize, Rgba::CYAN, CONSOLE_FONT_PATH);
	}
	else
	{
		renderer->drawText2D(bottomLeft, m_currentBuffer, m_textSize, Rgba::CYAN, CONSOLE_FONT_PATH);
	}
	//*/
	if (time % 2 == 0)
	{
		BitmapFont* bf = nullptr;
		bf = renderer->CreateOrGetBitmapFont(CONSOLE_BITMAP_FONT_NAME);
		//renderer->DrawText2D(bottomLeft, m_currentBuffer + ">", m_textSize, Rgba::CYAN, .55f, renderer->CreateOrGetBitmapFont(CONSOLE_BITMAP_FONT_NAME));
		renderer->DrawText2D(bottomLeft, m_currentBuffer, m_textSize, Rgba::CYAN, .55f, renderer->CreateOrGetBitmapFont(CONSOLE_BITMAP_FONT_NAME));
		float txtWitdh = bf->getTextWidth(std::string(m_currentBuffer.begin(), m_caretPos), m_textSize, .55f);
		renderer->drawText2D(Vector2(txtWitdh,.0f), "|", m_textSize, Rgba::WHITE, CONSOLE_FONT_PATH);
		//renderer->DrawText2D(Vector2(txtWitdh, .0f), "|", m_textSize, Rgba::WHITE, .55f);
	}
	else
		renderer->DrawText2D(bottomLeft, m_currentBuffer, m_textSize, Rgba::CYAN, .55f, renderer->CreateOrGetBitmapFont(CONSOLE_BITMAP_FONT_NAME));
	//*/
	renderer->translate2D(0.f, m_textSize);
	renderer->drawLine(Vector2::ZERO, Vector2(1280.f, 0.f), Rgba::WHITE);

	if(m_useBitmap)
		drawMsgs_BitmapFont(renderer);
	else
		drawMsgs_Font(renderer);
}

void Console::Render(Renderer* renderer) const
{
	Vector2 bottomLeft(0.f, 0.f);
	int time = (int)GetCurrentTimeSeconds();
	if (time % 2 == 0)
	{
		renderer->DrawText2D(bottomLeft, m_currentBuffer + ">", m_textSize, Rgba::CYAN, .75f, renderer->CreateOrGetFont(CONSOLE_BITMAP_FONT_NAME));
	}
	else
		renderer->DrawText2D(bottomLeft, m_currentBuffer, m_textSize, Rgba::CYAN, .75f, renderer->CreateOrGetFont(CONSOLE_BITMAP_FONT_NAME));
	for (int i = m_msgs.size() - 1; i >= 0; --i)
	{
		RenderableString msg = m_msgs[i];
		bottomLeft.y += (float)msg.m_scale;
		renderer->DrawText2D(bottomLeft, msg.m_string, msg.m_scale, msg.m_color, .75f, renderer->CreateOrGetFont(CONSOLE_BITMAP_FONT_NAME));
	}
}
//////////////////////////////////////////////////////////////////////////
void Console::addMessage(const RenderableString& msg)
{
	OnMessagePrint.trigger(msg.m_string);
	m_msgs.push_back(msg);
}

void Console::addMessage(const std::string& msg)
{
	RenderableString consoleMsg(msg, 1.f, Rgba::WHITE);
	addMessage(consoleMsg);
}

void Console::addMessage(const std::string& msg, float scale, const Rgba& color)
{
	RenderableString consoleMsg(msg, scale, color);
	addMessage(consoleMsg);
}

void Console::addMessage(const std::string& msg, const Rgba& color)
{
	RenderableString consoleMsg(msg, 1.f, color);
	addMessage(consoleMsg);
}

//////////////////////////////////////////////////////////////////////////
void Console::OnKeyPressed(unsigned char keyCode)
{
	if (m_caretPos != m_currentBuffer.end())
	{
		m_caretPos = m_currentBuffer.insert(m_caretPos, keyCode) + 1;
	}
	else
	{
		m_currentBuffer += keyCode;
		m_caretPos = m_currentBuffer.end();
	}
}

void Console::OnKeyCodePressed(unsigned char keyCode)
{
	if (keyCode == KEYCODE_TILDE)
	{
		g_consoleVisable = false;
	}
	else if (keyCode == KEYCODE_ESCAPE)
	{
		if (m_currentBuffer.empty())
		{
			g_consoleVisable = false;
		}
		else
		{
			m_currentBuffer.clear();
			m_caretPos = m_currentBuffer.end();
		}
	}
	else if (keyCode == KEYCODE_BACKSPACE)
	{
		if (!m_currentBuffer.empty())
		{
			if (m_caretPos != m_currentBuffer.end())
			{
				if (m_caretPos != m_currentBuffer.begin())
					m_caretPos = m_currentBuffer.erase(m_caretPos - 1);
			}
			else
			{
				m_currentBuffer.pop_back();
				--m_caretPos;
			}
		}
	}
	else if (keyCode == KEYCODE_UP)
	{
		if (m_typedCommands.empty())
			return;

		if (m_currentBuffer.empty())
			m_msgsPos = m_typedCommands.size() - 1;
		else if (m_msgsPos > 0)
			--m_msgsPos;

		m_currentBuffer = m_typedCommands[m_msgsPos];
		m_caretPos = m_currentBuffer.end();
	}
	else if (keyCode == KEYCODE_DOWN)
	{
		if (m_typedCommands.empty())
			return;

		if (m_msgsPos < (int)m_typedCommands.size() - 1)
			++m_msgsPos;
		m_currentBuffer = m_typedCommands[m_msgsPos];
		m_caretPos = m_currentBuffer.end();
	}
	else if(keyCode == KEYCODE_LEFT)
	{
		if (m_caretPos != m_currentBuffer.begin())
		{
			--m_caretPos;
		}
	}
	else if (keyCode == KEYCODE_RIGHT)
	{
		if (m_caretPos != m_currentBuffer.end())
		{
			++m_caretPos;
		}
	}
	else if(keyCode == KEYCODE_TAB)
	{
		const char* foundCmd = AutoCompleteCommandName(m_currentBuffer);
		if (foundCmd)
		{
			m_currentBuffer = foundCmd;
			m_caretPos = m_currentBuffer.end();
		}
	}
	else if (keyCode == KEYCODE_ENTER || keyCode == KEYCODE_NUMPADENTER)
	{
		CommandRun(m_currentBuffer);
		m_currentBuffer.clear();
		m_msgsPos = m_typedCommands.size() - 1;
		m_caretPos = m_currentBuffer.end();
	}
}
////////////////////////////////////////////////////////////////////////// Private Functions //////////////////////////////////////////////////////////////////////////
void Console::drawMsgs_Font(SimpleRenderer* renderer) const
{
	for (int i = m_msgs.size() - 1; i >= 0; --i)
	{
		RenderableString msg = m_msgs[i];
		float scale = msg.m_scale * m_textSize;
		renderer->drawText2D(Vector2::ZERO, msg.m_string, scale, msg.m_color, CONSOLE_FONT_PATH);
		renderer->translate2D(0, scale);
	}
}

void Console::drawMsgs_BitmapFont(SimpleRenderer* renderer) const
{
	for (int i = m_msgs.size() - 1; i >= 0; --i)
	{
		RenderableString msg = m_msgs[i];
		float scale = msg.m_scale * m_textSize;
		renderer->DrawText2D(Vector2::ZERO, msg.m_string, scale, msg.m_color, .65f, renderer->CreateOrGetBitmapFont(CONSOLE_BITMAP_FONT_NAME));
		renderer->translate2D(0, scale);
	}
}
////////////////////////////////////////////////////////////////////////// Commands //////////////////////////////////////////////////////////////////////////
COMMAND(cls, "Clears the screen of messages.")
{
	g_theConsole->m_msgs.clear();
}
COMMAND(cmd_txt_size, "Set the console's font size by typing in a float value.")
{
	g_theConsole->m_textSize = stof(arguments);
}
COMMAND(cmd_toggle_font, "Switch between bitmap or fnt to use in the console.")
{
	g_theConsole->m_useBitmap = !g_theConsole->m_useBitmap;
}