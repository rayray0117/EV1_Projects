#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/ProfilerReport.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
//////////////////////////////////////////////////////////////////////////
PerformanceCounter::PerformanceCounter()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	QueryPerformanceCounter((LARGE_INTEGER*)&initialTime);
	secondsPerCount = (1.0 / static_cast<double>(countsPerSecond));
}

double PerformanceCounter::GetCurrentSeconds()
{
	uint64 currentCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentCount);
	LONGLONG elapsedCountsSinceInitialTime = currentCount - initialTime;

	double currentSeconds = static_cast<double>(elapsedCountsSinceInitialTime) * secondsPerCount;
	return currentSeconds;
}
//////////////////////////////////////////////////////////////////////////
// This works by assuming we are destroying the whole tree and not just ourself from it
ProfilerNode::~ProfilerNode()
{
	SAFE_DELETE(first_child); //Delete my first child who'll delete their siblings for me.

	if (parent == nullptr) //If my parent is null then I know I shouldn't have siblings
		return;

	if (sibling_right != parent->first_child) //If my right sibling is not the first child then I know I haven't looped.
	{
		SAFE_DELETE(sibling_right); //Delete my right sibling who will delete the others
	}
}


void ProfilerNode::AddChild(ProfilerNode* newChild)
{
	if (first_child == nullptr)
	{
		first_child = newChild;
		first_child->sibling_right = first_child;
		first_child->sibling_left = first_child;
	}
	else
	{
		newChild->sibling_right = first_child;
		newChild->sibling_left = first_child->sibling_left;
		newChild->sibling_left->sibling_right = newChild;
		first_child->sibling_left = newChild;
	}
	
	newChild->parent = this;
}

const double ProfilerNode::getTimeSpentOnSelf() const
{
	double timeSpentOnSelf = getTimeTotal();

	if (first_child == nullptr)
		return timeSpentOnSelf;


	//Since you can't spend time with 2 kids at the same time, we can just subtract my total time by their total times
	//Subtract the favorite child's first, then check for their siblings.
	timeSpentOnSelf -= first_child->getTimeTotal();

	//Subtract siblings till we are back at the first child again
	ProfilerNode* currentKid = first_child->sibling_right;
	while (currentKid != first_child)
	{
		timeSpentOnSelf -= currentKid->getTimeTotal();
		currentKid = currentKid->sibling_right;
	}

	return timeSpentOnSelf;
}

////////////////////////////////////////////////////////////////////////// Profiler //////////////////////////////////////////////////////////////////////////
Profiler::Profiler()
	: Singleton()
	, bEnabled(false)
	, m_previousFrame(nullptr)
	, m_head(nullptr)
	, m_thread(nullptr)
{
	
}

Profiler::~Profiler()
{
	
}

void Profiler::StartUp()
{
#ifdef PROFILE_BUILD
	bEnabled = true;
	m_thread = ::GetCurrentThread();
#endif
}

void Profiler::Shutdown()
{
#ifdef PROFILE_BUILD
	bEnabled = false;
	SAFE_DELETE(m_previousFrame);
	SAFE_DELETE(m_head);
#endif
}

void Profiler::Render(SimpleRenderer* renderer) const
{
	if (isDisabled() || !bShouldRender) return;
	OnMainThreadCheck();


	renderer->SetBlendFunc(RHIEnum::BLEND_SOURCE_ALPHA, RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA);
	renderer->EnableDepth(false);
	renderer->BindShader(DEFAULT_UNLIT_SHADER);
	renderer->SetOrthoProjection(Vector2::ZERO, Vector2(1280, 720));

	ProfilerNode* prevFrame = getPreviousFrame();
	
	std::string fpsText = Stringf("FPS: %f", inverse_f(renderer->timeData.system_frame_time));
	if (bDrawWithDropShadow)
	{
		renderer->drawText2D(Vector2(10 - 2, 720 - 32 - 2), fpsText, 32.0f, Rgba::BLACK, "Data/Fonts/Garamound32.fnt");
	}
	renderer->drawText2D(Vector2(10, 720 - 32), fpsText, 32.0f, Rgba::WHITE, "Data/Fonts/Garamound32.fnt");

	std::string frameTimeText = Stringf("Frame time: %s", timeToString(renderer->timeData.system_time).c_str());
	if (bDrawWithDropShadow)
	{
		renderer->drawText2D(Vector2(10 - 2, 720 - 64 - 2), frameTimeText, 32.0f, Rgba::BLACK, "Data/Fonts/Garamound32.fnt");
	}
	renderer->drawText2D(Vector2(10, 720 - 64), frameTimeText, 32.0f, Rgba::WHITE, "Data/Fonts/Garamound32.fnt");

	if (prevFrame == nullptr)
		return;
	/*/
	std::string frameTimeText = Stringf("Last frame time: %s", timeToString(prevFrame->start_time).c_str());
	renderer->drawText2D(Vector2(10, 720 - 64), frameTimeText, 32.0f, Rgba::WHITE, "Data/Fonts/Garamound32.fnt");
	//*/
	ProfilerReport report(prevFrame);
	if (bUseFlatView)
		report.CreateFlatViewReport();
	else
		report.CreateTreeViewReport();

	for (uint i = 0; i < report.m_reportMessages.size(); ++i)
	{
		RenderableString msg = report.m_reportMessages[i];
		float scale = msg.m_scale * 20;
		//renderer->drawText2D(Vector2(10, 720 - 64 - scale * (i + 1)), msg.m_string, scale, msg.m_color, "Data/Fonts/Garamound32.fnt");
		if (bDrawWithDropShadow)
		{
			renderer->DrawText2D(Vector2(10 - 2, 720 - 64 - 2 - scale * (i + 1)), msg.m_string, scale, Rgba::BLACK, .65f, renderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
		}
		renderer->DrawText2D(Vector2(10, 720 - 64 - scale * (i + 1)), msg.m_string, scale, msg.m_color, .65f, renderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
	}
}

//////////////////////////////////////////////////////////////////////////
void Profiler::Push(char const *tag)
{
	if (isDisabled()) return;
	OnMainThreadCheck();

	ProfilerNode* newNode = new ProfilerNode;
	newNode->tag = tag;
	newNode->start_time = GetCurrentTimeSeconds();

	if (m_head != nullptr)
	{
		m_head->AddChild(newNode);
	}

	m_head = newNode;
}

void Profiler::Pop()
{
	if (isDisabled()) return;
	OnMainThreadCheck();
	ASSERT_OR_DIE(m_head != nullptr, "Profiler Pop was called on an empty tree");

	m_head->end_time = GetCurrentTimeSeconds();
	if (m_head->parent == nullptr)
	{
		PushPreviousFrame(m_head);
		m_head = nullptr;
	}
	else
	{
		m_head = m_head->parent;
	}
}

void Profiler::Pause()
{
	if (isDisabled()) return;
	OnMainThreadCheck();

	bProfilingPaused = true;
}

void Profiler::Resume()
{
	if (isDisabled()) return;
	OnMainThreadCheck();

	bProfilingPaused = false;
}

void Profiler::LogReport()
{
	if (isDisabled()) return;
	OnMainThreadCheck();


	ProfilerReport report(getPreviousFrame());

	if (bUseFlatView)
		report.CreateFlatViewReport();
	else
		report.CreateTreeViewReport();

	report.Log();
}

ProfilerNode* Profiler::getPreviousFrame() const
{
	if (isDisabled()) return nullptr;
	OnMainThreadCheck();


	return m_previousFrame;
}

//////////////////////////////////////////////////////////////////////////
const void Profiler::OnMainThreadCheck() const
{
	ASSERT_OR_DIE(m_thread == ::GetCurrentThread(), "Profiler was called on a different thread!");
}

void Profiler::PushPreviousFrame(ProfilerNode* prevFrame)
{
	if (bProfilingPaused)
	{
		SAFE_DELETE(prevFrame); //Delete it so we aren't leaking memory
		return;
	}
	SAFE_DELETE(m_previousFrame);
	m_previousFrame = prevFrame;
}

////////////////////////////////////////////////////////////////////////// COMMANDS //////////////////////////////////////////////////////////////////////////
#ifdef PROFILE_BUILD //Don't want these commands cluttering up help if there is profiler to use them for.
COMMAND(profile_log_report, "Prints a report about the last frame to the the log")
{
	Profiler::GetInstance()->LogReport();
}

COMMAND(profile_resume, "Resumes profiling")
{
	Profiler::GetInstance()->Resume();
}

COMMAND(profile_pause, "Pauses profiling")
{
	Profiler::GetInstance()->Pause();
}

COMMAND(profile_view_flat, "Sets profiler to show a flat view")
{
	Profiler::GetInstance()->bUseFlatView = true;
}

COMMAND(profile_view_tree, "Sets profiler to show a tree view")
{
	Profiler::GetInstance()->bUseFlatView = false;
}

COMMAND(profile_render, "Toggles profiler's ability to render")
{
	Profiler::GetInstance()->bShouldRender = !Profiler::GetInstance()->bShouldRender;
}
#endif