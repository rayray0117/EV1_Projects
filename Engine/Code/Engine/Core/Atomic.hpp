#pragma once
#include "Engine/Core/EngineBase.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
//////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
// Will return the result of the operation
FORCEINLINE uint AtomicAdd(uint volatile *ptr, uint const value)
{
	return (uint) ::InterlockedAddNoFence((LONG volatile*)ptr, (LONG)value);
}

//--------------------------------------------------------------------
FORCEINLINE uint AtomicIncrement(uint *ptr)
{
	return (uint) ::InterlockedIncrementNoFence((LONG volatile*)ptr);
}

//--------------------------------------------------------------------
FORCEINLINE uint AtomicDecrement(uint *ptr)
{
	return (uint) ::InterlockedDecrementNoFence((LONG volatile*)ptr);
}

//--------------------------------------------------------------------
FORCEINLINE uint CompareAndSet(uint volatile *ptr, uint const comparand, uint const value)
{
	/*
	uint const old_value = *ptr;
	if (old_value == comparand) {
	*ptr = value;
	}
	return old_value;
	*/

	return ::InterlockedCompareExchange(ptr, value, comparand);
}

//--------------------------------------------------------------------
/*/
FORCEINLINE bool CompareAndSet128(uint64 volatile data[2], uint64 comparand[2], uint64 value[2])
{
	return 1 == ::InterlockedCompareExchange128((long long volatile*)data, value[1], value[0], (long long*)comparand);
}
//*/
//--------------------------------------------------------------------
template <typename T>
FORCEINLINE T* CompareAndSetPointer(T *volatile *ptr, T *comparand, T *value)
{
	return (T*)::InterlockedCompareExchangePointerNoFence((PVOID volatile*)ptr, (PVOID)value, (PVOID)comparand);
}
