#include "Engine/Core/Logger.hpp"
#include <stdio.h>
#include <stdarg.h>
#define THREAD_SAFE_QUEUE
#include "Engine/Core/Thread.hpp"
#include <string>

#include "Engine/Core/Signal.hpp"
#include "Engine/Core/Event.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <set>
#include "Engine/Core/Time.hpp"
#include "JobSystem.hpp"
//////////////////////////////////////////////////////////////////////////
FILE* g_LogFile = nullptr;
const char* g_LogFilePath;
Thread::handle g_LoggerThread = nullptr;
static bool bIsLoggerThreadRunning = false;
Thread::Queue<std::string> g_Messages;
Event<const char*> g_LogEvent;
Signal g_LogSignal;
CriticalSection g_cs;

static bool bTagsEnabled = true; //True means all tags allow, list holds only disabled tags. False means all tags disable, list holds only disabled tags.
std::set<std::string> g_tags;
//////////////////////////////////////////////////////////////////////////
void LoggerThread(const char* filePath);
void LogFlushTest(const char* msg);
void Printf_ThreadSafeTest(int thread_count, int line_count);
bool TagIsAllowed(const char* tag);
//////////////////////////////////////////////////////////////////////////
void Log::StartUp(const char* filePath)
{
	g_LogFilePath = filePath;
	bIsLoggerThreadRunning = true;

	g_LoggerThread = Thread::Create(LoggerThread, filePath);
}

void Log::Shutdown()
{
	g_LogSignal.signal_all();
	bIsLoggerThreadRunning = false;
	Thread::Join(g_LoggerThread);
	g_LoggerThread = INVALID_THREAD_HANDLE;
	//SAFE_DELETE(g_LogFile);
}
//////////////////////////////////////////////////////////////////////////

void Log::Print(const char* messageFormat, ...)
{
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	int messageSize = vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);
	messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)
	UNUSED(messageSize);

	g_Messages.push(messageLiteral);
	g_LogSignal.signal_all();
}


void Log::TagPrint(const char* tag, const char* messageFormat, ...)
{
	//Cheap tag filter. Filter at print, not when adding tags.
	if (!TagIsAllowed(tag))
		return;

	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	int messageSize = vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);
	messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)
	UNUSED(messageSize);

	char line_buffer[MESSAGE_MAX_LENGTH];

	sprintf_s(line_buffer, MESSAGE_MAX_LENGTH, "[%s] [%s] %s\n", tag, GetLocalTime(), messageLiteral);

	g_Messages.push(line_buffer);
	g_LogSignal.signal_all();
}

void Log::Printf(const char* messageFormat, ...)
{
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	int messageSize = vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);
	messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)
	fwrite(messageLiteral, 1, messageSize, g_LogFile);
}

void Log::TagPrintf(const char* tag, const char* messageFormat, ...)
{
	if (g_LogFile == nullptr)
		return;

	if (tag == nullptr)
		tag = "default";


	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	int messageSize = vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);
	messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\n'; // In case vsnprintf overran (doesn't auto-terminate)

	fwrite("[", 1, 1, g_LogFile);
	fwrite(tag, 1, strlen(tag), g_LogFile);
	fwrite("] ", 1, 2, g_LogFile);
	fwrite(messageLiteral, 1, messageSize, g_LogFile);
}

void Log::WarningPrintf(const char* messageFormat, ...)
{
	TagPrintf("warning", messageFormat);
}

void Log::ErrorPrintf(const char* messageFormat, ...)
{
	TagPrintf("error", messageFormat);
	Flush();
	ERROR_AND_DIE(messageFormat);
}

void Log::PrintF_ThreadEventCall(void* /*user_arg*/, const char* event_arg)
{
	//std::string *msg = (std::string*)event_arg;
	Printf(/*msg->c_str()*/event_arg);
}

//////////////////////////////////////////////////////////////////////////
struct signal_test_t
{
	uint index;
	Signal *signal;
};
static bool gSignalTestRunning = false;
static void SignalTestThread(void *data)
{
	signal_test_t *test = (signal_test_t*)data;

	uint count = 0;
	while (gSignalTestRunning) {
		test->signal->wait();
		++count;
		Log::TagPrint("test","Thread[%i] was signaled [%i].\n", test->index, count);
	}

	delete test;
}

//////////////////////////////////////////////////////////////////////////
void Log::Flush()
{
	//const char* msg;
	std::string msg;

	
}

void Log::WriteToFile()
{
	std::string str;
	if (g_Messages.pop(&str))
	{
		g_cs.lock();
		if (g_LogFile)
		{
			fwrite(str.data(), 1, str.size(), g_LogFile);
		}
		g_cs.unlock();
	}
}

void Log::EnableAllTags()
{
	bTagsEnabled = true;
	g_tags.clear();
}

void Log::DisableAllTags()
{
	bTagsEnabled = false;
	g_tags.clear();
}

void Log::EnableTag(const char* tag)
{
	if (bTagsEnabled) //If all tags are allowed then remove it from the list
	{
		g_tags.erase(tag);
		return;
	}
		

	g_tags.insert(tag);
}

void Log::DisableTag(const char* tag)
{
	if (bTagsEnabled) //If no tags are allowed then remove it from the list
	{
		g_tags.insert(tag);
	}
	else
	{
		g_tags.erase(tag);
		return;
	}
}

////////////////////////////////////////////////////////////////////////// Locals //////////////////////////////////////////////////////////////////////////
bool TagIsAllowed(const char* tag)
{
	if (bTagsEnabled)
	{
		auto found = g_tags.find(tag);
		if (found != g_tags.end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		auto found = g_tags.find(tag);
		if (found != g_tags.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void LoggerThread(const char* filePath)
{
	g_LogFile = nullptr;
 	errno_t err = fopen_s(&g_LogFile, filePath, "w+");
 	if ((err != 0) || (g_LogFile == nullptr)) {
 		return;
 	}

	JobConsumer log_consumer;
	log_consumer.AddCategory(JOB_LOGGING);
	JobSystemSetCategorySignal(JOB_LOGGING, &g_LogSignal);


	while (bIsLoggerThreadRunning)
	{
		g_LogSignal.wait();
		while (!g_Messages.empty())
		{
			//Pr
		}

		Log::Flush();

		log_consumer.ConsumeAll();
	}

	Log::Flush();
	fclose(g_LogFile);

	const int MESSAGE_MAX_LENGTH = 512;
	char newFileName[MESSAGE_MAX_LENGTH];
	sprintf_s(newFileName, MESSAGE_MAX_LENGTH, "%s_%s\0", filePath, GetLocalTime());
	CopyFile(filePath, newFileName, true);
}

void LogFlushTest(const char* msg)
{
	Log::Print(msg);
	Log::Flush();
	ASSERT_OR_DIE(false, "Testing stuff");;
}


////////////////////////////////////////////////////////////////////////// COMMANDS //////////////////////////////////////////////////////////////////////////
#include "Engine/Core/Command.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/EngineConfig.hpp"

COMMAND(log_thread_test, "")
{
	std::vector<std::string> buffer;
	splitArgs(arguments, buffer);

	if (buffer.size() > 1)
	{
		Thread::handle th = Thread::Create(Printf_ThreadSafeTest, stoi(buffer[0]), stoi(buffer[1]));
		Thread::Detach(th);
	}
		
}

COMMAND(log_flush_test, "")
{
	LogFlushTest(arguments.c_str());
}

COMMAND(log_flush, "flushes out the log")
{
	Log::Flush();
}

////////////////////////////////////////////////////////////////////////// COMMAND LOCAL FUNCTIONS //////////////////////////////////////////////////////////////////////////
void Printf_ThreadSafeTest(int thread_count, int line_count)
{
	g_theConsole->addMessage("PrintF Thread Safe Test Started");

	Signal signal;

	gSignalTestRunning = true;

	for (int i = 0; i < thread_count; ++i) {
		signal_test_t *test = new signal_test_t();
		test->index = i;
		test->signal = &signal;

		// thread will handle deleting the variable.
		Thread::handle hand = Thread::Create(SignalTestThread, test);
		Thread::Detach(hand);
	}

	// give all the threads a chance to startup.
	// not guaranteed, but good enough for a test.
	Thread::Sleep(50);

	// now, signal them 50 times - do all go at once, or 
	// do they go one at a time?
	for (int i = 0; i < line_count; ++i) {
		signal.signal_all();
		Thread::Sleep(100);
	}

	gSignalTestRunning = false;
	signal.signal_all();

	// not the safest - but just wait till everyone has exited.
	Thread::Sleep(100);
	g_theConsole->addMessage("PrintF Thread Safe Test Finished");
}

COMMAND(log_enable_all_tags, "")
{
	Log::EnableAllTags();
}
COMMAND(log_disable_all_tags, "")
{
	Log::DisableAllTags();
}
COMMAND(log_enable_tag, "")
{
	Log::EnableTag(arguments.c_str());
}
COMMAND(log_disable_tag, "")
{
	Log::DisableTag(arguments.c_str());
}

void LogCopyTest(const std::string& filename)
{
	g_theConsole->addMessage("LogCopyTest started");
	Log::Flush();
	fclose(g_LogFile);
	CopyFile(g_LogFilePath, filename.c_str(), true);
	fopen_s(&g_LogFile, g_LogFilePath, "a+");
	g_theConsole->addMessage("LogCopyTest finished");
}

COMMAND(log_copy, "Param: Filename")
{
	Job::Run(JOB_LOGGING, LogCopyTest, arguments);
}