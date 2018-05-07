#pragma once
#include "Engine/Input/InputSystem.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
//////////////////////////////////////////////////////////////////////////
extern InputSystem* g_inputSystem;
extern App* g_theApp;
extern Game* g_theGame;

class Window;
extern Window* g_myWindow;

class SimpleRenderer;
extern SimpleRenderer* g_renderer;
class AudioSystem;
extern AudioSystem* g_audioSystem;

class Config;
extern Config* g_gameConfig;
//////////////////////////////////////////////////////////////////////////
// Config System stuff
constexpr char* WINDOW_HEIGHT_KEY = "window_height";
constexpr char* WINDOW_WIDTH_KEY = "window_width";
extern int g_windowHeight;
extern int g_windowWidth;

constexpr char* FRAMERATE_KEY = "framerate";
extern double g_targetTime;


constexpr char* NUM_IMAGES_KEY = "num_images";
extern int g_numImages;
constexpr char* IMAGE_KEY = "image_";

constexpr char* NUM_SHADERS_KEY = "num_shaders";
extern int g_numShaders;
constexpr char* SHADER_KEY = "shader_";

constexpr char* MENU_MUSIC_KEY = "menu_music";
extern std::string g_menuMusic;


void setupGameConfig(const char* filename);
float getAspectRatio();
//////////////////////////////////////////////////////////////////////////
constexpr char* XML_TILEDEF_FILEPATH = "Data/Gameplay/Tiles.xml";
constexpr char* XML_ITEMDEF_FILEPATH = "Data/Gameplay/Items.xml";
constexpr char* XML_MAPDEF_FILEPATH = "Data/Gameplay/Maps.xml";
constexpr char* XML_PAWNDEF_FILEPATH = "Data/Gameplay/Pawns.xml";
constexpr char* XML_FEATUREDEF_FILEPATH = "Data/Gameplay/Features.xml";
constexpr char* XML_ADVENDEF_FILEPATH = "Data/Gameplay/Adventures.xml";
constexpr char* SPRITE_BITMAP_FONT_NAME = "Bisasam_16x16";
constexpr char* DEFAULT_BITMAP_FONT_NAME = "SquirrelFixedFont";
extern std::string g_spriteBitmap;
extern std::string g_bitmapFont;
//////////////////////////////////////////////////////////////////////////
constexpr char FLOATING_NUMBER_END_ALPHA = 0;
constexpr float FLOATING_NUMBER_END_HEIGHT = .5f;
constexpr float FLOATING_NUMBER_DURATION = 1.25f;
constexpr float FLOATING_NUMBER_TEXT_HEIGHT = .25f;
constexpr float FLOATING_NUMBER_TEXT_ASPECT_RATIO = .75f;
//////////////////////////////////////////////////////////////////////////
class Map;
//////////////////////////////////////////////////////////////////////////
Map* getCurrentMap();

extern float g_musicVolume;
extern float g_sfxVolume;
void PlayMusic(const std::string filePath, float volume = 1.f);
void PlaySFX(const std::string filePath, float volume = 1.f);
//////////////////////////////////////////////////////////////////////////
enum InputName
{
	INPUT_UP,
	INPUT_DOWN,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_START,
	INPUT_SELECT,
	INPUT_X,
	INPUT_CIRCLE,
	INPUT_SQAURE,
	INPUT_TRIANGLE,
	INPUT_R1,
	INPUT_L1,

	INPUT_ENTER,
	INPUT_BACK = INPUT_SELECT,
	INPUT_ACCEPT = INPUT_ENTER, // GPad_A, Enter
	INPUT_CANCEL,
	
	INPUT_LMB,

	INPUT_CAMERA,
	INPUT_REST,
	INPUT_STATSCREEN,
	NUM_INPUT_NAMES
};

bool wasInputJustPressed(InputName name);
/*bool isDown(InputName name);*/