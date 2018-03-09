#include "Game/GameCommon.hpp"
#include "Engine/RHI/RHI.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Profiler.hpp"
//////////////////////////////////////////////////////////////////////////

App* g_theApp = nullptr;
Game* g_theGame = nullptr;
InputSystem* g_inputSystem = nullptr;

Window* g_myWindow;
SimpleRenderer* g_renderer = nullptr;
Config* g_gameConfig = nullptr;


double g_targetTime = 1.0 / 60.0;
int g_windowHeight = 480;
int g_windowWidth = 640;
int g_numImages = 0;
int g_numShaders = 0;

float CAMERA_MOVEMENT_SPEED = 1.f;

float g_fovVerticalDegrees = 60.f;
float g_nearClip = 0.1f;
float g_farClip = 100.f;
//////////////////////////////////////////////////////////////////////////
void setupGameConfig(const char* filename)
{
	g_gameConfig->ConfigLoadFile(filename);

	g_gameConfig->ConfigGetInt(WINDOW_HEIGHT_KEY, g_windowHeight);
	g_gameConfig->ConfigGetInt(WINDOW_WIDTH_KEY, g_windowWidth);
	g_gameConfig->ConfigGetInt(NUM_IMAGES_KEY, g_numImages);
	g_gameConfig->ConfigGetInt(NUM_SHADERS_KEY, g_numShaders);

	g_gameConfig->ConfigGetFloat(FOV_KEY, g_fovVerticalDegrees);
	g_gameConfig->ConfigGetFloat(NEAR_CLIP_KEY, g_nearClip);
	g_gameConfig->ConfigGetFloat(FAR_CLIP_KEY, g_farClip);

	float framerate = 60.0;
	if (g_gameConfig->ConfigGetFloat(FRAMERATE_KEY, framerate))
		g_targetTime = (double)inverse_f(framerate);

	g_gameConfig->ConfigGetBool("profile_render", Profiler::GetInstance()->bShouldRender);
}

float getAspectRatio()
{
	return (float)g_windowWidth / (float)g_windowHeight;
}

