#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/World.hpp"
#include "Game/Map.hpp"

InputSystem* g_inputSystem = nullptr;
App* g_theApp = nullptr;
Game* g_theGame = nullptr;


Window* g_myWindow = nullptr;
SimpleRenderer* g_renderer = nullptr;
AudioSystem* g_audioSystem = nullptr;
Config* g_gameConfig = nullptr;



int g_windowHeight = 480;
int g_windowWidth = 640;
double g_targetTime = 1.0 / 60.0;
int g_numImages = 0;
int g_numShaders = 0;
std::string g_menuMusic;

std::string g_spriteBitmap = SPRITE_BITMAP_FONT_NAME;
std::string g_bitmapFont = DEFAULT_BITMAP_FONT_NAME;



Map* getCurrentMap()
{
    return g_theGame->m_theWorld->mo_currentMap;
}
//////////////////////////////////////////////////////////////////////////
bool wasInputJustPressed(InputName name)
{
	const XboxController& controller = g_inputSystem->GetControllerState(0);
	switch (name)
	{
	case INPUT_UP: 
		return g_inputSystem->wasKeyJustPressed(KEYCODE_UP) || g_inputSystem->wasKeyJustPressed(KEYCODE_W) || controller.wasButtonJustPressed(GAMEPAD_DPAD_UP);
	case INPUT_DOWN:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_DOWN) || g_inputSystem->wasKeyJustPressed(KEYCODE_S) || controller.wasButtonJustPressed(GAMEPAD_DPAD_DOWN);
	case INPUT_LEFT:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_LEFT) || g_inputSystem->wasKeyJustPressed(KEYCODE_A) || controller.wasButtonJustPressed(GAMEPAD_DPAD_LEFT);
	case INPUT_RIGHT:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_RIGHT) || g_inputSystem->wasKeyJustPressed(KEYCODE_D) || controller.wasButtonJustPressed(GAMEPAD_DPAD_RIGHT);

	case INPUT_SELECT:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_BACKSPACE) || controller.wasButtonJustPressed(GAMEPAD_BACK);
	case INPUT_START:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_ESCAPE) || controller.wasButtonJustPressed(GAMEPAD_START);

	case INPUT_X:
		return controller.wasButtonJustPressed(GAMEPAD_A) || g_inputSystem->wasKeyJustPressed(KEYCODE_LMB);
	case INPUT_CIRCLE:
		return controller.wasButtonJustPressed(GAMEPAD_B) || g_inputSystem->wasKeyJustPressed(KEYCODE_RMB);
	case INPUT_SQAURE:
		return controller.wasButtonJustPressed(GAMEPAD_X);
	case INPUT_TRIANGLE:
		return controller.wasButtonJustPressed(GAMEPAD_Y);
	case INPUT_R1:
		return controller.wasButtonJustPressed(GAMEPAD_RIGHT_SHOULDER) || g_inputSystem->wasKeyJustPressed(KEYCODE_PAGEDOWN);
	case INPUT_L1:
		return controller.wasButtonJustPressed(GAMEPAD_LEFT_SHOULDER) || g_inputSystem->wasKeyJustPressed(KEYCODE_PAGEUP);

	case INPUT_ENTER:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_ENTER) || controller.wasButtonJustPressed(GAMEPAD_A);
	case INPUT_CANCEL:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_BACKSPACE) || controller.wasButtonJustPressed(GAMEPAD_B);
	case INPUT_LMB:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_LMB);
	

	case INPUT_CAMERA:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_C) || controller.wasButtonJustPressed(GAMEPAD_RIGHT_THUMB);
	case INPUT_REST:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_SPACE) || controller.wasButtonJustPressed(GAMEPAD_X);
	case INPUT_STATSCREEN:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_BACKSPACE) || g_inputSystem->wasKeyJustPressed(KEYCODE_P) || g_inputSystem->wasKeyJustPressed(KEYCODE_TAB) || controller.wasButtonJustPressed(GAMEPAD_BACK);
	default: 
		return false;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void setupGameConfig(const char* filename)
{
	g_gameConfig->ConfigLoadFile(filename);

	g_gameConfig->ConfigGetInt(WINDOW_HEIGHT_KEY, g_windowHeight);
	g_gameConfig->ConfigGetInt(WINDOW_WIDTH_KEY, g_windowWidth);
	g_gameConfig->ConfigGetInt(NUM_IMAGES_KEY, g_numImages);
	g_gameConfig->ConfigGetInt(NUM_SHADERS_KEY, g_numShaders);

	float framerate = 60;
	if(g_gameConfig->ConfigGetFloat(FRAMERATE_KEY, framerate))
		g_targetTime = (double)inverse_f(framerate);

	g_gameConfig->ConfigGetString(MENU_MUSIC_KEY, g_menuMusic);
}

float getAspectRatio()
{
	return (float)g_windowWidth / (float)g_windowHeight;
}
//////////////////////////////////////////////////////////////////////////
float g_musicVolume = 0.6f;
float g_sfxVolume = 1.f;

void PlayMusic(const std::string filePath, float volume /*= 1.f*/)
{
	if (filePath.empty())
		return;

	g_audioSystem->PlayMusic(filePath, g_musicVolume * volume);
}

void PlaySFX(const std::string filePath, float volume /*= 1.f*/)
{
	if (filePath.empty())
		return;
	SoundID s = g_audioSystem->CreateOrGetSound(filePath);
	g_audioSystem->PlaySound(s, volume * g_sfxVolume);
}