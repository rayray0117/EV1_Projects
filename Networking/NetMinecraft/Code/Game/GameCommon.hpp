#pragma once
#include "Engine/Input/InputSystem.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector3.hpp"
//////////////////////////////////////////////////////////////////////////
// class App;
// class Game;
// class Renderer;
// class InputSystem;
//////////////////////////////////////////////////////////////////////////
extern App* g_theApp;
extern InputSystem* g_inputSystem;
//extern Renderer* g_renderer;
extern Game* g_theGame;
//////////////////////////////////////////////////////////////////////////
class GameSession;
class Window;
class SimpleRenderer;
class Config;
class AudioSystem;
//////////////////////////////////////////////////////////////////////////
extern GameSession* g_gameSession;
extern Window* g_myWindow;
extern SimpleRenderer* g_renderer;
extern Config* g_gameConfig;
extern AudioSystem* g_audioSystem;
//////////////////////////////////////////////////////////////////////////
bool operator < (IntVector2 left, IntVector2 right);
//////////////////////////////////////////////////////////////////////////
enum BlockGenerationType
{
	FLAT,
	CUSTOM,
	PERLIN,
	NUM_BLOCK_GENERATION_TYPES
};

enum BlockTypes : unsigned char
{
	AIR,
	GRASS,
	STONE,
	DIRT,
	SAND,
	WATER,
	COBBLESTONE,
	GLOWSTONE,
	NUM_BLOCK_TYPES
};

enum CameraModes : int
{
	FIRST_PERSON,
	FROM_BEHIND,
	FIXED_ANGLE,
	NO_CLIP
};
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

constexpr char* NET_UPDATE_KEY = "net_hz";
extern float NET_UPDATE_HZ;

extern char* g_savePath;

void setupGameConfig(const char* filename);
float getAspectRatio();
//////////////////////////////////////////////////////////////////////////
constexpr char* FOV_KEY = "fov";
constexpr char* NEAR_CLIP_KEY = "near_clip";
constexpr char* FAR_CLIP_KEY = "far_clip";
extern float g_fovVerticalDegrees;
extern float g_nearClip;
extern float g_farClip;
extern int g_Seed;

extern BlockGenerationType g_BlockGenerationType;
extern int g_skyLightValue;
extern Rgba g_skyColor;
extern float g_sfxVolume;
extern const char* g_musicPath;
extern float g_musicVolume;
//////////////////////////////////////////////////////////////////////////
// Camera Consts
extern float CAMERA_MOVEMENT_SPEED;
const float CAMERA_SPRINT_MULTIPIER = 8.f;
const float CAMERA_HORIZONTAL_TURN_RATE = 45.f;
const float CAMERA_VERTICAL_TURN_RATE = 45.f;
const float MOUSE_SENSITIVITY_X = 0.1f;
const float MOUSE_SENSITIVITY_Y = 0.1f;
const float CONTROLLER_SENSITIVITY_X = 270.f;
const float CONTROLLER_SENSITIVITY_Y = 360.f;
//////////////////////////////////////////////////////////////////////////
constexpr float GRAVITY = 9.8f;
const Vector3 GRAVITY_ACCELERATION(0.f, 0.f, -GRAVITY);
constexpr float JUMP_POWER = 0.5f;
const Vector3 JUMP_VELOCITY(0.f, 0.f, GRAVITY * JUMP_POWER);
constexpr float PLAYER_MOVEMENT_SPEED = 5.00f;
constexpr float FRICTION = PLAYER_MOVEMENT_SPEED*10.f;
//////////////////////////////////////////////////////////////////////////
constexpr unsigned char CHUNK_X_BITS = 3;
constexpr unsigned char CHUNK_Y_BITS = 3;
constexpr unsigned char CHUNK_Z_BITS = 4;
constexpr int CHUNK_XY_BITS = CHUNK_X_BITS + CHUNK_Y_BITS;
constexpr int CHUNK_X = BIT(CHUNK_X_BITS);
constexpr int CHUNK_Y = BIT(CHUNK_Y_BITS);
constexpr int CHUNK_Z = BIT(CHUNK_Z_BITS);
constexpr int X_MASK_BITS = CHUNK_X - 1;
constexpr int Y_MASK_BITS = (CHUNK_Y - 1) << CHUNK_X_BITS;
constexpr int Z_MASK_BITS = (CHUNK_Z - 1) << CHUNK_XY_BITS;
constexpr int BLOCKS_PER_LAYER = CHUNK_X * CHUNK_Y;
constexpr int BLOCKS_PER_CHUNK = BLOCKS_PER_LAYER * CHUNK_Z;
//////////////////////////////////////////////////////////////////////////
constexpr int HALF_CHUNK_X = (int)(CHUNK_X * .5f);
constexpr int HALF_CHUNK_Y = (int)(CHUNK_Y * .5f);
constexpr int HALF_CHUNK_Z = (int)(CHUNK_Z * .5f);
//////////////////////////////////////////////////////////////////////////
constexpr int STONE_CHUNK_HEIGHT = 2;
constexpr int DIRT_CHUNK_HEIGHT = 4;
constexpr int GRASS_CHUNK_HEIGHT = DIRT_CHUNK_HEIGHT + 1;
constexpr int SEA_LEVEL = 3;
//////////////////////////////////////////////////////////////////////////
constexpr int MAX_NUM_OF_CHUNKS = /*200;*/3000;
constexpr int IDEAL_NUM_OF_CHUNKS = 1200;
constexpr float MAX_CHUNK_VIS_DIST = 70.f; // is in meters (1 block)
constexpr int CHUNK_VIEW_DIST = 8;
constexpr int MAX_RLE_VIEW_DIST = 10;
constexpr int NUM_VERTICES_TO_RESERVE = 10000;// BLOCKS_PER_CHUNK * 24
//////////////////////////////////////////////////////////////////////////
constexpr float RAYCAST_DIST = 8.f;
constexpr float RAYCAST_DIST_INVERSE = 1.0f / RAYCAST_DIST;
//////////////////////////////////////////////////////////////////////////
constexpr unsigned char VERSION = 0;
//////////////////////////////////////////////////////////////////////////
constexpr char* DEFAULT_FONT = "SquirrelFixedFont";
//////////////////////////////////////////////////////////////////////////
constexpr unsigned char LIGHT_MASK = 0b00001111;
constexpr unsigned char IS_SKY_MASK = 0b10000000;
constexpr unsigned char LIGHT_DIRTY_MASK = 0b01000000;
constexpr unsigned char OPAQUE_MASK = 0b00100000;
constexpr unsigned char SOLID_MASK = 0b00010000;
constexpr unsigned char OPAQUE_AND_SOLID_MASK = OPAQUE_MASK | SOLID_MASK;
//////////////////////////////////////////////////////////////////////////
constexpr int DAYLIGHT = 15;
constexpr int MOONLIGHT = 6;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
const std::string SFX_GRASS_DIG = "Data/Audio/SFX/dig/grass";
constexpr int	SFX_GRASS_DIG_NUM = 8;
const std::string SFX_GRASS_STEP = "Data/Audio/SFX/step/grass";
constexpr int	SFX_GRASS_STEP_NUM = 6;

const std::string SFX_SAND_DIG = "Data/Audio/SFX/dig/sand";
constexpr int	SFX_SAND_DIG_NUM = 4;
const std::string SFX_SAND_STEP = "Data/Audio/SFX/step/sand";
constexpr int	SFX_SAND_STEP_NUM = 5;

const std::string SFX_STONE_DIG = "Data/Audio/SFX/dig/stone";
constexpr int	SFX_STONE_DIG_NUM = 4;
const std::string SFX_STONE_STEP = "Data/Audio/SFX/step/stone";
constexpr int	SFX_STONE_STEP_NUM = 6;

const std::string SFX_WOOD_DIG = "Data/Audio/SFX/dig/wood";
constexpr int	SFX_WOOD_DIG_NUM = 5;
const std::string SFX_WOOD_STEP = "Data/Audio/SFX/step/wood";
constexpr int	SFX_WOOD_STEP_NUM = 6;

const std::string SFX_SWIM_STEP = "Data/Audio/SFX/liquid/swim";
constexpr int	SFX_SWIM_STEP_NUM = 4;

const std::string SFX_SPLASH = "Data/Audio/SFX/liquid/splash";
constexpr int	SFX_SPLASH_NUM = 3;

const std::string SFX_BREAK = "Data/Audio/SFX/random/break";
constexpr int	SFX_BREAK_NUM = 1;

const std::string SFX_GLASS_DIG = "Data/Audio/SFX/random/glass";
constexpr int	SFX_GLASS_DIG_NUM = 3;

const std::string SFX_POP = "Data/Audio/SFX/liquid/lavapop";
constexpr int	SFX_POP_NUM = 1;
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
	INPUT_SHOOT,
	INPUT_STATSCREEN,
	NUM_INPUT_NAMES
};

bool wasInputJustPressed(InputName name);
bool wasInputJustReleased(InputName name);
bool isDown(InputName name); 
//////////////////////////////////////////////////////////////////////////
inline unsigned char getByteBrightnessForBlockLighting(unsigned char blockLighting)
{
	static const unsigned char colorByteLookupForBlockLight[16] =
	{
		20, 30, 40, 50, 60, 70, 80, 100, 120, 140, 160, 180, 200, 220, 240, 255
	};
	return colorByteLookupForBlockLight[blockLighting];
}

inline int getBlockIndexWithMinZForBlockIndex(int blockIndex)
{
	//ASSERT_OR_DIE in bounds
	return (blockIndex & ~Z_MASK_BITS);/*blockCoords.x + (blockCoords.y*CHUNK_X);*/
}

inline int getBlockIndexWithMaxZForBlockIndex(int blockIndexXY)
{
	return (blockIndexXY | Z_MASK_BITS);
}

inline void incrementCameraMode(CameraModes& cam)
{
	if (cam == NO_CLIP)
		cam = FIRST_PERSON;
	else
		cam = (CameraModes)(cam + 1);
}

inline void incrementPlacementBlock(BlockTypes& cam)
{
	cam = (BlockTypes)(cam + 1);

	if (cam == NUM_BLOCK_TYPES)
		cam = (BlockTypes)0;
}

inline void decrementPlacementBlock(BlockTypes& cam)
{
	if (cam == (BlockTypes)0)
		cam = (BlockTypes)((int)NUM_BLOCK_TYPES - 1);
	else
		cam = (BlockTypes)(cam - 1);


}
void loadTidus();
void drawTidus();