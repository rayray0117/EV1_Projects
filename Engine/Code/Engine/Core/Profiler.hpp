#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Singleton.hpp"
//////////////////////////////////////////////////////////////////////////
class SimpleRenderer;
//////////////////////////////////////////////////////////////////////////
// Made GetCurrentPerformanceCounter and PerformanceCounterToSeconds its own struct because QueryPerformanceFrequency only needs to be called once for a Counter
// Also Squirrels GetCurrentTimeSeconds covers everything I'd probably use those for, so this is made to have individualized time trackers. Not really sure if I need them.
struct PerformanceCounter
{
	PerformanceCounter();

	double GetCurrentSeconds();

	uint64 initialTime;
	uint64 countsPerSecond;
	double secondsPerCount;
};
struct ProfilerNode
{
public:
	~ProfilerNode();
	
	void AddChild(ProfilerNode* newChild);

	const double getTimeTotal() const
	{
		return end_time - start_time;
	}
	const double getTimeSpentOnSelf() const;
public:
	const char* tag;
	double start_time;
	double end_time;
	ProfilerNode* parent = nullptr;
	ProfilerNode* first_child = nullptr;
	ProfilerNode* sibling_left = nullptr;
	ProfilerNode* sibling_right = nullptr;
};
//////////////////////////////////////////////////////////////////////////
class Profiler : public Singleton<Profiler>
{
	friend Singleton;
public:
	Profiler();
	~Profiler();
	
	void StartUp();
	void Shutdown();
	void Render(SimpleRenderer* renderer) const;

	void Push(char const *tag);
	void Pop();

	void Pause();
	void Resume();

	void LogReport();

	ProfilerNode* getPreviousFrame() const;
	
	FORCEINLINE const bool isEnabled() const 
	{
		return bEnabled;
	}
	FORCEINLINE const bool isDisabled() const
	{
		return !bEnabled;
	}
public:
	bool bUseFlatView = false;
	bool bDrawWithDropShadow = false; //Enabling this will render the text twice; once for the shadow then again for the actual text.
	bool bShouldRender = true; //Allows me to turn the draw on and off through the console.
private:
	const void OnMainThreadCheck() const;
	void PushPreviousFrame(ProfilerNode* prevFrame);
private:
	bool bEnabled = false; //Because Profiler is singleton, it can always be created if ever called so check this variable to see if it was actually started up.
	void* m_thread;
	ProfilerNode* m_head;
	ProfilerNode* m_previousFrame;
	bool bProfilingPaused = false;
};


//////////////////////////////////////////////////////////////////////////
class ScopedProfiler
{
public:
	ScopedProfiler(const char* tag)
	{
		Profiler* p = Profiler::GetInstance();
		if (p->isDisabled())
			return;

		p->Push(tag);
	}
	~ScopedProfiler()
	{
		Profiler* p = Profiler::GetInstance();
		if (p->isDisabled())
			return;

		p->Pop();
	}
};
////////////////////////////////////////////////////////////////////////// MACROS //////////////////////////////////////////////////////////////////////////
#define PROFILE_SCOPE(tag_str) ScopedProfiler __pscope_##__LINE__##(tag_str)
#define PROFILE_SCOPE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)