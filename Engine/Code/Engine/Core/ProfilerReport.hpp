#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Renderer/RenderableString.hpp"
#include <vector>
#include <map>
//////////////////////////////////////////////////////////////////////////
struct ProfilerNode;
//////////////////////////////////////////////////////////////////////////
struct ProfilerReportInfo
{
public:
	const char* tag = nullptr;

	uint num_calls = 0;

	float percent_total = 0;
	double time_total = 0;

	float percent_self = 0;
	double time_self = 0;

	double average_total = 0;
	double average_self = 0;
public:
	void CombineWithNode(ProfilerNode* node);
	void setPercent(double totalFrameTime_i); //Send inverse_d(totalFrameTime)
	void setAverages();
	std::string toString(uint numIndents) const;
};

struct ProfilerReportTreeHolder
{
public:
	ProfilerReportInfo info;
	//ProfilerReportTreeHolder* parent;
	std::map<const char*, ProfilerReportTreeHolder> kids;
public:
	void CombineWithNode(ProfilerNode* node);
	void PushKid(ProfilerNode* node);
	void setPercentAndAverages(double totalFrameTime_i); //Send inverse_d(totalFrameTime)
};
//////////////////////////////////////////////////////////////////////////
class ProfilerReport
{
public:
	ProfilerReport(ProfilerNode* frame);
	~ProfilerReport();

	void AddReportMessage(std::string msg);

	void CreateFlatViewReport();
	void PushNodeInfo_Flat(ProfilerNode* frame);

	void CreateTreeViewReport();
	void PrintTreeNode(const ProfilerReportTreeHolder& tree, uint treeDepth);

	void Log();
	void ConsolePrint();
public:
	ProfilerNode* mo_frame;
	std::vector<RenderableString> m_reportMessages;
	std::map<const char*, ProfilerReportInfo> m_flatViewReport;
	ProfilerReportTreeHolder m_treeViewReport;
};