#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/TemplateArgs.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
enum EJobType : uint
{
	JOB_GENERIC,
	JOB_MAIN,
	JOB_IO,
	JOB_RENDER,
	JOB_LOGGING,
	NUM_JOB_TYPES
};
//////////////////////////////////////////////////////////////////////////


class Job;
void JobDispatchAndRelease(Job *job);


typedef void(*job_work_cb)(void*);

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class Job
{
public:
	EJobType type;
	job_work_cb work_cb;

	void *user_data;

	std::vector<Job*> dependents;
	uint num_dependencies;

public:
	void OnFinish();
	void OnDependancyFinished();

	void DependentOn(Job *parent);


	static Job* Create(EJobType type, job_work_cb work_cb, void *user_data);
	//*/
	template <typename CB, typename ...ARGS>
	static Job* Create(EJobType type, CB entry_point, ARGS ...args)
	{
		pass_data_t<CB, ARGS...> *pass = new pass_data_t<CB, ARGS...>(entry_point, args...);
		return Create(type, ForwardArguments<CB, ARGS...>, (void*)pass);
	}
	//*/

	template <typename CB, typename ...ARGS>
	static void Run(EJobType type, CB entry_point, ARGS ...args)
	{
		Job* jobToRun = Create(type, entry_point, args...);
		JobDispatchAndRelease(jobToRun);
	}
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class JobConsumer
{
public:
	void AddCategory(uint category);
	bool ConsumeJob();
	uint ConsumeAll();

public:
	std::vector<EJobType> m_categories;
};

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/
void JobSystemStartup(uint job_category_count, int generic_thread_count = -1);
void JobSystemShutdown();
class Signal;
void JobSystemSetCategorySignal(EJobType type, Signal* sig);
//Job* JobCreate(EJobType type, job_work_cb work_cb, void *user_data);
void JobDispatchAndRelease(Job *job);


//------------------------------------------------------------------------
// THIS SHOULD BE MOVED TO A JOB CONSUMER OBJECT!
uint JobConsumeAll(EJobType type);
