#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Callstack.hpp"
//////////////////////////////////////////////////////////////////////////
namespace MemTrac
{
	struct AllocationData
	{
		size_t byte_size;
		AllocationData* left;
		AllocationData* right;
		Callstack* cs;
		size_t timeAllocated;

		void Destroy();
		const bool isValid() const { return (right != nullptr && left != nullptr); }
	};

	void ProfileMemoryFrameTick();
	uint getAllocationCount();
	size_t getAllocationByteCount();
	uint getFrameAllocationCount();
	uint getFrameFreeCount();
	size_t getHighWater();

	void LogLiveAllocations();
}
//////////////////////////////////////////////////////////////////////////
void* operator new(const size_t size);
void operator delete(void* ptr);