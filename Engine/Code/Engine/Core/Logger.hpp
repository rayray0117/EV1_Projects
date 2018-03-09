#pragma once
#include "Engine/Core/EngineBase.hpp"
//#include <string>
//////////////////////////////////////////////////////////////////////////
namespace Log
{
	FORCENOINLINE void StartUp(const char* filePath);
	FORCENOINLINE void Shutdown();

	void Print(const char* messageFormat, ...);
	void TagPrint(const char* tag, const char* messageFormat, ...);
	void Printf(const char* messageFormat, ...);
	void TagPrintf(const char* tag, const char* messageFormat, ...);
	void WarningPrintf(const char* messageFormat, ...);
	void ErrorPrintf(const char* messageFormat, ...);
	void PrintF_ThreadEventCall(void *user_arg, const char* event_arg);

	void Flush();
	void WriteToFile();

	void EnableAllTags();
	void DisableAllTags();
	void EnableTag(const char* tag);
	void DisableTag(const char* tag);
}