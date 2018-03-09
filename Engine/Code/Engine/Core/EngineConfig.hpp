#pragma once
#include "Engine/Core/Memory.hpp"

#define RENDER_DEBUG
//////////////////////////////////////////////////////////////////////////
class Console;
extern Console* g_theConsole;
extern bool g_consoleVisable;

class SimpleRenderer;
extern SimpleRenderer* g_mainEngineRenderer;
//////////////////////////////////////////////////////////////////////////
#define TRACK_MEMORY_BASIC		0;
#define TRACK_MEMORY_VERBOSE	1;

#if defined(_DEBUG)
	#define TRACK_MEMORY	TRACK_MEMORY_VERBOSE;
	#define PROFILE_BUILD
#elif defined(FINAL_BUILD)
	
#else
	//#define TRACK_MEMORY TRACK_MEMORY_BASIC;
	#define PROFILE_BUILD
#endif