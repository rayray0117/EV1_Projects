#include "Engine/Core/Memory.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Callstack.hpp"
#include "ErrorWarningAssert.hpp"
//////////////////////////////////////////////////////////////////////////
static uint g_AllocCount = 0;
static uint g_FrameAllocs = 0;
static uint g_FrameFrees = 0;
static uint g_PrevFrameAllocs = 0;
static uint g_PrevFrameFrees = 0;
static size_t g_AllocByteCount = 0;
static size_t g_HighWater = 0;
static MemTrac::AllocationData* g_AllocationList = nullptr;
//////////////////////////////////////////////////////////////////////////
void MemTrac::ProfileMemoryFrameTick()
{
	g_PrevFrameAllocs = g_FrameAllocs;
	g_PrevFrameFrees = g_FrameFrees;

	g_FrameAllocs = 0;
	g_FrameFrees = 0;
}

uint MemTrac::getAllocationCount()
{
	return g_AllocCount;
}

size_t MemTrac::getAllocationByteCount()
{
	return g_AllocByteCount;
}

uint MemTrac::getFrameAllocationCount()
{
	return g_PrevFrameAllocs;
}

uint MemTrac::getFrameFreeCount()
{
	return g_PrevFrameFrees;
}

size_t MemTrac::getHighWater()
{
	return g_HighWater;
}

void MemTrac::LogLiveAllocations()
{
	AllocationData* current = g_AllocationList;
	if (!current)
		return;

	MakeCallstackReport(current->cs);
	while (current->right != g_AllocationList)
	{
		current = current->right;
		MakeCallstackReport(current->cs);
	}
}
//*/
//////////////////////////////////////////////////////////////////////////
#include <malloc.h>
//////////////////////////////////////////////////////////////////////////
void* operator new(const size_t size)
{
#ifdef TRACK_MEMORY
	++g_AllocCount;
	++g_FrameAllocs;
	g_AllocByteCount += size;

	if (g_AllocByteCount > g_HighWater)
		g_HighWater = g_AllocByteCount;

	size_t alloc_size = size + sizeof(MemTrac::AllocationData);
	MemTrac::AllocationData* newPtr = (MemTrac::AllocationData*) malloc(alloc_size);
	newPtr->byte_size = size;

	int verbose = TRACK_MEMORY;
	if (verbose == 1)
	{
		newPtr->left = nullptr;
		newPtr->right = nullptr;
		newPtr->cs = CreateCallstack(1);
		newPtr->timeAllocated = (size_t)GetCurrentTimeSeconds();

		MemTrac::AllocationData* current = g_AllocationList;
		if (current == nullptr)
		{
			g_AllocationList = newPtr;
			newPtr->right = g_AllocationList;
			newPtr->left = g_AllocationList;
		}
		else
		{
			MemTrac::AllocationData* left = g_AllocationList->left;
			left->right = newPtr;
			newPtr->left = left;

			g_AllocationList->left = newPtr;
			newPtr->right = g_AllocationList;
		}
	}

	return newPtr + 1;

#else
	return malloc(size);
#endif
}

void operator delete(void* ptr)
{
#ifdef TRACK_MEMORY
	if (!ptr)
		return;


	--g_AllocCount;
	++g_FrameFrees;

	MemTrac::AllocationData* ptrToRemove = (MemTrac::AllocationData*) ptr;
	ptrToRemove--;

	ASSERT_OR_DIE(g_AllocByteCount > ptrToRemove->byte_size, "Freed memory is more than what is known on it!");
	g_AllocByteCount -= ptrToRemove->byte_size;

	int verbose = TRACK_MEMORY;
	if (verbose == 1)
	{
		if (ptrToRemove == g_AllocationList)
		{
			g_AllocationList = g_AllocationList->right;
			if (ptrToRemove == g_AllocationList)
			{
				g_AllocationList = nullptr;
			}
		}

		ptrToRemove->Destroy();
		if (g_AllocationList == nullptr || !g_AllocationList->isValid())
		{
			g_AllocationList = nullptr;
		}
	}

	free(ptrToRemove);
#else
	free(ptr);
#endif // TRACK_MEMORY
}

////////////////////////////////////////////////////////////////////////// COMMANDS //////////////////////////////////////////////////////////////////////////
#include "Engine/Core/Command.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"

COMMAND(mem_trac, "displays memory tracking stats")
{
	std::string allocCountText = Stringf("Allocation Count = %u", g_AllocCount);
	std::string allocByteCountText = Stringf("Allocation Byte Size = %s", bytesToString(g_AllocByteCount).c_str());
	std::string frameCountText = Stringf("Frame Allocations = %u", g_FrameAllocs);
	std::string frameFreeText = Stringf("Frame Frees = %u", g_FrameFrees);
	std::string highWaterTxt = Stringf("Highwater Size: %s", bytesToString(MemTrac::getHighWater()).c_str());

	g_theConsole->addMessage(allocCountText);
	g_theConsole->addMessage(allocByteCountText);
	g_theConsole->addMessage(frameCountText);
	g_theConsole->addMessage(frameFreeText);
	g_theConsole->addMessage(highWaterTxt);
}

#ifdef TRACK_MEMORY
COMMAND(mem_log, "Will print out the currect allocations")
{
	MemTrac::LogLiveAllocations();
}
#endif // 
//*/

void MemTrac::AllocationData::Destroy()
{
	left->right = right;
	right->left = left;

	left = nullptr;
	right = nullptr;
	DestroyCallstack(cs);
}
