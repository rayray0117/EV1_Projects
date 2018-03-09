#include "Engine/Core/ProfilerReport.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
//////////////////////////////////////////////////////////////////////////
const float CONSOLE_PROFILE_REPORT_SCALE = .85f;
const Rgba CONSOLE_PROFILE_REPORT_COLOR = Rgba(255, 235, 255, 255);
const int TAG_ALIGNMENT				= -40;
const int CALLS_ALIGNMENT			= 5;
const int PERCENT_TOTAL_ALIGNMENT	= 8;
const int TIME_TOTAL_ALIGNMENT		= 12;
const int PERCENT_SELF_ALIGNMENT	= 8;
const int TIME_SELF_ALIGNMENT		= 12;
const int AVERAGE_TOTAL_ALIGNMENT	= 12;
const int AVERAGE_SELF_ALIGNMENT	= 10;
const char* TAB = "  ";
//////////////////////////////////////////////////////////////////////////
void ProfilerReportInfo::CombineWithNode(ProfilerNode* node)
{
	tag = node->tag;
	++num_calls;
	time_total += node->getTimeTotal();
	time_self += node->getTimeSpentOnSelf();
}

void ProfilerReportInfo::setPercent(double totalFrameTime_i)
{
	percent_total = float(time_total * totalFrameTime_i) * 100.f;
	percent_self = float(time_self * totalFrameTime_i) * 100.f;
}

void ProfilerReportInfo::setAverages()
{
	average_self = time_self * inverse_d((double)num_calls);
	average_total = time_total * inverse_d((double)num_calls);
}

std::string ProfilerReportInfo::toString(uint numIndents) const
{
	std::string tabs = "";
	for (uint i = 0; i < numIndents; ++i)
	{
		tabs += TAB;
	}

	std::string tabbedTag = tabs + tag;

	 std::string str = Stringf("%*s %*i %*.2f %*s %*.2f %*s %*s %*s",
		TAG_ALIGNMENT,					tabbedTag.c_str(),
		CALLS_ALIGNMENT,				num_calls,
		PERCENT_TOTAL_ALIGNMENT,		percent_total,
		TIME_TOTAL_ALIGNMENT,			timeToString(time_total).c_str(),
		PERCENT_SELF_ALIGNMENT,			percent_self,
		TIME_SELF_ALIGNMENT,			timeToString(time_self).c_str(),
		AVERAGE_TOTAL_ALIGNMENT,		timeToString(average_total).c_str(),
		AVERAGE_SELF_ALIGNMENT,			timeToString(average_self).c_str());

	 return str;
}
//////////////////////////////////////////////////////////////////////////
void ProfilerReportTreeHolder::CombineWithNode(ProfilerNode* node)
{
	info.CombineWithNode(node);

	//If I don't have kids then I'm done
	if (node->first_child == nullptr)
		return;

	// I have one kid so push the first one 
	PushKid(node->first_child);

	ProfilerNode* currentKid = node->first_child->sibling_right;
	while (currentKid != node->first_child)
	{
		PushKid(currentKid);
		currentKid = currentKid->sibling_right;
	}
}

void ProfilerReportTreeHolder::PushKid(ProfilerNode* node)
{
	//See if I'm already on the list and add me
	auto found = kids.find(node->tag);
	if (found == kids.end())
	{
		ProfilerReportTreeHolder kid;
		kid.CombineWithNode(node);
		kids[kid.info.tag] = kid;
	}
	else
	{
		ProfilerReportTreeHolder& kid = found->second;
		kid.CombineWithNode(node);
	}
}

void ProfilerReportTreeHolder::setPercentAndAverages(double totalFrameTime_i)
{
	info.setPercent(totalFrameTime_i);
	info.setAverages();

	for (auto& currentKid : kids)
	{
		currentKid.second.setPercentAndAverages(totalFrameTime_i);
	}
}

////////////////////////////////////////////////////////////////////////// Profiler Report //////////////////////////////////////////////////////////////////////////
ProfilerReport::ProfilerReport(ProfilerNode* frame)
	: mo_frame(frame)
{
	std::string str = Stringf("%*s %*s %*s %*s %*s %*s %*s %*s",
		TAG_ALIGNMENT, "TAG NAME",
		CALLS_ALIGNMENT, "CALLS",
		PERCENT_TOTAL_ALIGNMENT, "TOTAL",
		TIME_TOTAL_ALIGNMENT, "TOTAL TIME",
		PERCENT_SELF_ALIGNMENT, "SELF",
		TIME_SELF_ALIGNMENT, "SELF TIME",
		AVERAGE_TOTAL_ALIGNMENT, "AVG TOTAL",
		AVERAGE_SELF_ALIGNMENT, "AVG SELF");


	AddReportMessage(str);
}

ProfilerReport::~ProfilerReport()
{

}
//////////////////////////////////////////////////////////////////////////
void ProfilerReport::AddReportMessage(std::string msg)
{
	RenderableString str(msg, CONSOLE_PROFILE_REPORT_SCALE, CONSOLE_PROFILE_REPORT_COLOR);
	m_reportMessages.push_back(str);
}

void ProfilerReport::CreateFlatViewReport()
{
	double totalFrameTime = mo_frame->getTimeTotal(); //Assume that the first node we got was the head and its total is 100%.

	PushNodeInfo_Flat(mo_frame);


	double totalFrameTime_i = inverse_d(totalFrameTime);
	for (auto& currentInfo : m_flatViewReport)
	{
		currentInfo.second.setPercent(totalFrameTime_i);
		currentInfo.second.setAverages();
	}

	for (auto& currentInfo : m_flatViewReport)
	{
		AddReportMessage(currentInfo.second.toString(0));
	}
}

void ProfilerReport::PushNodeInfo_Flat(ProfilerNode* frame)
{
	//See if I'm already on the list and add me
	auto found = m_flatViewReport.find(frame->tag);
	if (found == m_flatViewReport.end())
	{
		ProfilerReportInfo info;
		info.CombineWithNode(frame);
		m_flatViewReport[info.tag] = info;
	}
	else
	{
		ProfilerReportInfo& info = found->second;
		info.CombineWithNode(frame);
	}


	//Now push my kids info

	//Check if I have a kid, if not we are done
	if (frame->first_child == nullptr)
		return;

	//I have at least one kid so push them then check for others
	PushNodeInfo_Flat(frame->first_child);
	ProfilerNode* currentKid = frame->first_child->sibling_right;
	while (currentKid != frame->first_child)
	{
		PushNodeInfo_Flat(currentKid);
		currentKid = currentKid->sibling_right;
	}
}

void ProfilerReport::CreateTreeViewReport()
{
	double totalFrameTime = mo_frame->getTimeTotal(); //Assume that the first node we got was the head and its total is 100%.


	m_treeViewReport.CombineWithNode(mo_frame);

	double totalFrameTime_i = inverse_d(totalFrameTime);
	m_treeViewReport.setPercentAndAverages(totalFrameTime_i);

	PrintTreeNode(m_treeViewReport, 0);
}

void ProfilerReport::PrintTreeNode(const ProfilerReportTreeHolder& tree, uint treeDepth)
{
	AddReportMessage(tree.info.toString(treeDepth));

	for (const auto& currentKid : tree.kids)
	{
		PrintTreeNode(currentKid.second, treeDepth+1);
	}
}

void ProfilerReport::Log()
{
	for (uint i = 0; i < m_reportMessages.size(); ++i)
	{
		Log::TagPrint("profiler", m_reportMessages[i].m_string.c_str());
	}
}

void ProfilerReport::ConsolePrint()
{
	for (uint i = 0; i < m_reportMessages.size(); ++i)
	{
		g_theConsole->addMessage(m_reportMessages[i]);
	}
}
