#pragma once
#include "Time.hpp"
#include "ErrorWarningAssert.hpp"
#include "Logger.hpp"
//////////////////////////////////////////////////////////////////////////
class ProflieLogScope
{
public:
	ProflieLogScope(const char* scopeName);
	~ProflieLogScope();
private:
	const char* m_scopeName;
	double		m_timeAtStart;
};

inline ProflieLogScope::ProflieLogScope(const char* scopeName)
	: m_scopeName(scopeName)
	, m_timeAtStart(0.f)
{
	m_timeAtStart = GetCurrentTimeSeconds();
}

inline ProflieLogScope::~ProflieLogScope()
{
	double elapsedSeconds = GetCurrentTimeSeconds() - m_timeAtStart;
	Log::TagPrint("profile","ProfileLogScope  \"%s\" took %.02f ms\n", m_scopeName, 1000 * elapsedSeconds);
	DebuggerPrintf("ProfileLogScope \"%s\" took %.02f ms\n", m_scopeName, 1000 * elapsedSeconds);
}
////////////////////////////////////////////////////////////////////////// MACRO //////////////////////////////////////////////////////////////////////////

#define PROFILE_LOG_SCOPE(msg) ProflieLogScope __pscope_##__LINE__##(msg)