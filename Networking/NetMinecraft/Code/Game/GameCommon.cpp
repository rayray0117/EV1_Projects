#include "Game/GameCommon.hpp"
#include "Engine/RHI/RHI.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/ObjLoader.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////

App* g_theApp = nullptr;
Game* g_theGame = nullptr;
InputSystem* g_inputSystem = nullptr;

GameSession* g_gameSession = nullptr;
Window* g_myWindow;
SimpleRenderer* g_renderer = nullptr;
Config* g_gameConfig = nullptr;
AudioSystem* g_audioSystem = nullptr;

//////////////////////////////////////////////////////////////////////////
bool operator<(IntVector2 left, IntVector2 right)
{
	if (left.x < right.x)
		return true;
	if (left.x > right.x)
		return false;
	if (left.y < right.y)
		return true;

	return false;
}
//////////////////////////////////////////////////////////////////////////
double g_targetTime = 1.0 / 60.0;
int g_windowHeight = 480;
int g_windowWidth = 640;
int g_numImages = 0;
int g_numShaders = 0;

int g_Seed = 0;
float CAMERA_MOVEMENT_SPEED = 20.f;
float NET_UPDATE_HZ = 20.f;
//////////////////////////////////////////////////////////////////////////
float g_fovVerticalDegrees = 60.f;
float g_nearClip = 0.1f;
float g_farClip = 80.f;

char* g_savePath = "Data/SaveSlot0/";
const char* g_musicPath = "Data/Audio/Music/Cipher2.mp3";
float g_musicVolume = 0.6f;
//////////////////////////////////////////////////////////////////////////
BlockGenerationType g_BlockGenerationType = PERLIN;
int g_skyLightValue = DAYLIGHT; /*MOONLIGHT;*/
Rgba g_skyColor = Rgba::SKY_DAYLIGHT;/* Rgba::SKY_MOONLIGHT;*/
float g_sfxVolume = .375;
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

	float framerate = 60.0f;
	if (g_gameConfig->ConfigGetFloat(FRAMERATE_KEY, framerate))
		g_targetTime = (double)inverse_f(framerate);

	if (!g_gameConfig->ConfigGetFloat(NET_UPDATE_KEY, NET_UPDATE_HZ))
		NET_UPDATE_HZ = 20.f;
}

float getAspectRatio()
{
	return (float)g_windowWidth / (float)g_windowHeight;
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
	case INPUT_SHOOT:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_SPACE) || controller.wasButtonJustPressed(GAMEPAD_X);
	case INPUT_STATSCREEN:
		return g_inputSystem->wasKeyJustPressed(KEYCODE_BACKSPACE) || g_inputSystem->wasKeyJustPressed(KEYCODE_P) || g_inputSystem->wasKeyJustPressed(KEYCODE_TAB) || controller.wasButtonJustPressed(GAMEPAD_BACK);
	default:
		return false;
		break;
	}
}

bool wasInputJustReleased(InputName name)
{
	const XboxController& controller = g_inputSystem->GetControllerState(0);
	switch (name)
	{
	case INPUT_UP:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_UP) || g_inputSystem->wasKeyJustReleased(KEYCODE_W) || controller.wasButtonJustReleased(GAMEPAD_DPAD_UP);
	case INPUT_DOWN:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_DOWN) || g_inputSystem->wasKeyJustReleased(KEYCODE_S) || controller.wasButtonJustReleased(GAMEPAD_DPAD_DOWN);
	case INPUT_LEFT:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_LEFT) || g_inputSystem->wasKeyJustReleased(KEYCODE_A) || controller.wasButtonJustReleased(GAMEPAD_DPAD_LEFT);
	case INPUT_RIGHT:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_RIGHT) || g_inputSystem->wasKeyJustReleased(KEYCODE_D) || controller.wasButtonJustReleased(GAMEPAD_DPAD_RIGHT);

	case INPUT_SELECT:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_BACKSPACE) || controller.wasButtonJustReleased(GAMEPAD_BACK);
	case INPUT_START:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_ESCAPE) || controller.wasButtonJustReleased(GAMEPAD_START);

	case INPUT_X:
		return controller.wasButtonJustReleased(GAMEPAD_A) || g_inputSystem->wasKeyJustReleased(KEYCODE_LMB);
	case INPUT_CIRCLE:
		return controller.wasButtonJustReleased(GAMEPAD_B) || g_inputSystem->wasKeyJustReleased(KEYCODE_RMB);
	case INPUT_SQAURE:
		return controller.wasButtonJustReleased(GAMEPAD_X);
	case INPUT_TRIANGLE:
		return controller.wasButtonJustReleased(GAMEPAD_Y);
	case INPUT_R1:
		return controller.wasButtonJustReleased(GAMEPAD_RIGHT_SHOULDER) || g_inputSystem->wasKeyJustReleased(KEYCODE_PAGEDOWN);
	case INPUT_L1:
		return controller.wasButtonJustReleased(GAMEPAD_LEFT_SHOULDER) || g_inputSystem->wasKeyJustReleased(KEYCODE_PAGEUP);

	case INPUT_ENTER:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_ENTER) || controller.wasButtonJustReleased(GAMEPAD_A);
	case INPUT_CANCEL:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_BACKSPACE) || controller.wasButtonJustReleased(GAMEPAD_B);
	case INPUT_LMB:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_LMB);


	case INPUT_CAMERA:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_C) || controller.wasButtonJustReleased(GAMEPAD_RIGHT_THUMB);
	case INPUT_SHOOT:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_SPACE) || controller.wasButtonJustReleased(GAMEPAD_X);
	case INPUT_STATSCREEN:
		return g_inputSystem->wasKeyJustReleased(KEYCODE_BACKSPACE) || g_inputSystem->wasKeyJustReleased(KEYCODE_P) || g_inputSystem->wasKeyJustReleased(KEYCODE_TAB) || controller.wasButtonJustReleased(GAMEPAD_BACK);
	default:
		return false;
		break;
	}
}

bool isDown(InputName name)
{
	const XboxController& controller = g_inputSystem->GetControllerState(0);
	switch (name)
	{
	case INPUT_UP:
		return g_inputSystem->isKeyDown(KEYCODE_UP) || g_inputSystem->isKeyDown(KEYCODE_W) || controller.isButtonDown(GAMEPAD_DPAD_UP);
	case INPUT_DOWN:
		return g_inputSystem->isKeyDown(KEYCODE_DOWN) || g_inputSystem->isKeyDown(KEYCODE_S) || controller.isButtonDown(GAMEPAD_DPAD_DOWN);
	case INPUT_LEFT:
		return g_inputSystem->isKeyDown(KEYCODE_LEFT) || g_inputSystem->isKeyDown(KEYCODE_A) || controller.isButtonDown(GAMEPAD_DPAD_LEFT);
	case INPUT_RIGHT:
		return g_inputSystem->isKeyDown(KEYCODE_RIGHT) || g_inputSystem->isKeyDown(KEYCODE_D) || controller.isButtonDown(GAMEPAD_DPAD_RIGHT);

	case INPUT_SELECT:
		return g_inputSystem->isKeyDown(KEYCODE_BACKSPACE) || controller.isButtonDown(GAMEPAD_BACK);
	case INPUT_START:
		return g_inputSystem->isKeyDown(KEYCODE_ESCAPE) || controller.isButtonDown(GAMEPAD_START);

	case INPUT_X:
		return controller.isButtonDown(GAMEPAD_A) || g_inputSystem->isKeyDown(KEYCODE_LMB);
	case INPUT_CIRCLE:
		return controller.isButtonDown(GAMEPAD_B) || g_inputSystem->isKeyDown(KEYCODE_RMB);
	case INPUT_SQAURE:
		return controller.isButtonDown(GAMEPAD_X);
	case INPUT_TRIANGLE:
		return controller.isButtonDown(GAMEPAD_Y);
	case INPUT_R1:
		return controller.isButtonDown(GAMEPAD_RIGHT_SHOULDER) || g_inputSystem->isKeyDown(KEYCODE_PAGEDOWN);
	case INPUT_L1:
		return controller.isButtonDown(GAMEPAD_LEFT_SHOULDER) || g_inputSystem->isKeyDown(KEYCODE_PAGEUP);

	case INPUT_ENTER:
		return g_inputSystem->isKeyDown(KEYCODE_ENTER) || controller.isButtonDown(GAMEPAD_A);
	case INPUT_CANCEL:
		return g_inputSystem->isKeyDown(KEYCODE_BACKSPACE) || controller.isButtonDown(GAMEPAD_B);
	case INPUT_LMB:
		return g_inputSystem->isKeyDown(KEYCODE_LMB);


	case INPUT_CAMERA:
		return g_inputSystem->isKeyDown(KEYCODE_C) || controller.isButtonDown(GAMEPAD_RIGHT_THUMB);
	case INPUT_SHOOT:
		return g_inputSystem->isKeyDown(KEYCODE_SPACE) || controller.isButtonDown(GAMEPAD_X);
	case INPUT_STATSCREEN:
		return g_inputSystem->isKeyDown(KEYCODE_BACKSPACE) || g_inputSystem->isKeyDown(KEYCODE_P) || g_inputSystem->isKeyDown(KEYCODE_TAB) || controller.isButtonDown(GAMEPAD_BACK);
	default:
		return false;
		break;
	}
}

VertexBuffer* objVBO;
unsigned int objVBOSize;

void loadTidus()
{
	std::vector<Vertex3D> objVArray;
	LoadOBJ("Data/Objs/model.obj", objVArray);
	objVBO = g_renderer->CreateAndGetVBOStatic(objVArray);
	objVBOSize = objVArray.size();
}

void drawTidus()
{
	g_renderer->pushMatrix();
	g_renderer->scale3DUniform(.001f);
	g_renderer->BindTexture("ColorSpectrum");
	g_renderer->drawVBO3D(objVBO, objVBOSize, RHIEnum::PRIMATIVE_TRIANGLE);
}
