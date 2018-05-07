#include "Engine/Core/Thread.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include "Engine/Core/Command.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/JobSystem.hpp"
//////////////////////////////////////////////////////////////////////////
struct thread_pass_data_t
{
	thread_cb cb;
	void *arg;
};
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI ThreadEntryPointCommon(void *arg);
void WriteJunkDataToFile(const char* filename, size_t byte_count);
void GenerateGarbage(const char* filename, size_t byte_count);
//////////////////////////////////////////////////////////////////////////

Thread::handle Thread::Create(thread_cb cb, void* data)
{
	// handle is like pointer, or reference to a thread
	// thread_id is unique identifier
	thread_pass_data_t *pass = new thread_pass_data_t();
	pass->cb = cb;
	pass->arg = data;

	DWORD thread_id;
	handle th = ::CreateThread(nullptr,   // SECURITY OPTIONS
		0,                         // STACK SIZE, 0 is default
		ThreadEntryPointCommon,    // "main" for this thread
		pass,                     // data to pass to it
		0,                         // initial flags
		&thread_id);              // thread_id

	return th;
}

void Thread::Sleep(uint ms)
{
	::Sleep(ms);
}

void Thread::Yield_()
{
	::SwitchToThread();
}

void Thread::Detach(handle th)
{
	::CloseHandle(th);
}

void Thread::Join(handle th)
{
	::WaitForSingleObject(th, INFINITE);
	::CloseHandle(th);
}

void Thread::Demo(char const *path, uint byte_size)
{
	g_theConsole->addMessage("Starting Thread Demo");
	GenerateGarbage(path, byte_size);
}

////////////////////////////////////////////////////////////////////////// Local Functions //////////////////////////////////////////////////////////////////////////
DWORD WINAPI ThreadEntryPointCommon(void *arg)
{
	thread_pass_data_t *pass_ptr = (thread_pass_data_t*)arg;

	pass_ptr->cb(pass_ptr->arg);
	delete pass_ptr;
	return 0;
}

void GenerateGarbage(const char* filename, size_t byte_count)
{
	/*/
	Thread::handle th = Thread::Create(WriteJunkDataToFile, filename, byte_count);
	Thread::Detach(th);
	//*/
	Job::Run(JOB_GENERIC, WriteJunkDataToFile, filename, byte_count);
	//JobDispatchAndRelease(jobToRun);
}

void WriteJunkDataToFile(const char* filename, size_t byte_count)
{
	FILE* fp = nullptr;
	errno_t errorCode = fopen_s(&fp, filename, "w");
	if (errorCode != 0)
		return;

	fseek(fp, byte_count, SEEK_SET);
	fputc('\0', fp);
	fclose(fp);

	g_theConsole->addMessage("Finished writing junk for Threading Demo");
}

////////////////////////////////////////////////////////////////////////// COMMANDS //////////////////////////////////////////////////////////////////////////
/*/
COMMAND(thread_run_demo, "Runs the thread test demo. Optional Params: filePath, byteSize")
{
	std::vector<std::string> buffer;
	splitArgs(arguments, buffer);

	if (buffer.size() > 1)
		Thread::Demo(buffer[0].c_str(), (uint)stoi(buffer[1]));
	else if (buffer.size() == 1)
		Thread::Demo(buffer[0].c_str(), 200 MiB);
	else
		Thread::Demo("Data/ThreadTest.dat", 200 MiB);
}//*/