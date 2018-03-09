#pragma once
#include "Engine/Input/InputSystem.hpp"
//#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
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
class Window;
extern Window* g_myWindow;

class SimpleRenderer;
extern SimpleRenderer* g_renderer;

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

constexpr char* FOV_KEY = "fov";
constexpr char* NEAR_CLIP_KEY = "near_clip";
constexpr char* FAR_CLIP_KEY = "far_clip";
extern float g_fovVerticalDegrees;
extern float g_nearClip;
extern float g_farClip;

constexpr char* CAM_SPEED_KEY = "cam_speed";
constexpr char* DEBUG_END_SPEED_KEY = "debug_end_speed";
constexpr char* MODEL_KEY = "start_model";

void setupGameConfig(const char* filename);
float getAspectRatio();
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
constexpr unsigned char VERSION = 0;
//////////////////////////////////////////////////////////////////////////
constexpr int CHICKEN_NUM_VERTS = 192;