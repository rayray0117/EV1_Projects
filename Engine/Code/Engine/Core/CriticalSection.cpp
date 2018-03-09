#include "Engine/Core/CriticalSection.hpp"
//////////////////////////////////////////////////////////////////////////
CriticalSection::CriticalSection()
{
	::InitializeCriticalSection(&cs);
}

CriticalSection::~CriticalSection()
{
	::DeleteCriticalSection(&cs);
}
//////////////////////////////////////////////////////////////////////////
void CriticalSection::lock()
{
	::EnterCriticalSection(&cs);
}

bool CriticalSection::try_lock()
{
	return TRUE == ::TryEnterCriticalSection(&cs);
}

void CriticalSection::unlock()
{
	::LeaveCriticalSection(&cs);
}

////////////////////////////////////////////////////////////////////////// Scoped //////////////////////////////////////////////////////////////////////////
ScopeCriticalSection::ScopeCriticalSection(CriticalSection *ptr)
{
	cs_ptr = ptr;
	cs_ptr->lock();
}

ScopeCriticalSection::~ScopeCriticalSection()
{
	cs_ptr->unlock();
}