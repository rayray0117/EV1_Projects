#include "Engine/Core/JobSystem.hpp"
#define THREAD_SAFE_QUEUE
#include "Engine/Core/Thread.hpp"
#include "Engine/Core/Signal.hpp"
#include "Engine/Core/Atomic.hpp"
#include "Engine/Core/Singleton.hpp"
//////////////////////////////////////////////////////////////////////////
class JobSystem : public Singleton<JobSystem>
{
	friend Singleton;
public:
	Thread::Queue<Job*> *queues;
	Signal **signals;
	uint queue_count;

	bool is_running;

	JobConsumer* generic_consumer;
	//Signal* generic_signal;
};
//////////////////////////////////////////////////////////////////////////
static void GenericJobThread(void*)
{
	JobConsumer *generic_consumer = JobSystem::GetInstance()->generic_consumer;
	Signal *generic_signal = JobSystem::GetInstance()->signals[JOB_GENERIC];

	while (JobSystem::GetInstance()->is_running) 
	{
		generic_consumer->ConsumeAll();
		generic_signal->wait();
	}

	generic_consumer->ConsumeAll();
}

//////////////////////////////////////////////////////////////////////////
void Job::OnFinish()
{
	for (uint i = 0; i < dependents.size(); ++i) 
	{
		dependents[i]->OnDependancyFinished();
	}
}

void Job::OnDependancyFinished()
{
	JobDispatchAndRelease(this);
}

void Job::DependentOn(Job *parent)
{
	AtomicIncrement(&num_dependencies);
	parent->dependents.push_back(this);
}

Job* Job::Create(EJobType type, job_work_cb work_cb, void *user_data)
{
	Job *job = new Job();
	job->type = type;
	job->work_cb = work_cb;
	job->user_data = user_data;
	job->num_dependencies = 1;

	return job;
}
//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
void JobSystemStartup(uint job_category_count, int generic_thread_count /*= -1*/)
{
	int core_count = (int)std::thread::hardware_concurrency();
	if (generic_thread_count <= 0) 
	{
		core_count += generic_thread_count;
	}
	core_count--; // one is always being created - so subtract from total wanted;

	// We need queues! 
	JobSystem::GetInstance()->queues = new Thread::Queue<Job*>[job_category_count];
	JobSystem::GetInstance()->signals = new Signal*[job_category_count];
	JobSystem::GetInstance()->queue_count = job_category_count;
	JobSystem::GetInstance()->is_running = true;
	JobConsumer *generic_consumer = new JobConsumer();
	generic_consumer->AddCategory(JOB_GENERIC);
	JobSystem::GetInstance()->generic_consumer = generic_consumer;


	for (uint i = 0; i < job_category_count; ++i) 
	{
		JobSystem::GetInstance()->signals[i] = nullptr;
	}

	// create the signal
	JobSystem::GetInstance()->signals[JOB_GENERIC] = new Signal();

	Thread::Create(GenericJobThread, JobSystem::GetInstance()->signals[JOB_GENERIC]);
	for (int i = 0; i < core_count; ++i) 
	{
		Thread::Create(GenericJobThread, JobSystem::GetInstance()->signals[JOB_GENERIC]);
	}
}

//------------------------------------------------------------------------
void JobSystemShutdown()
{
	// dont' forget to clean up.
	// TODO!
}


void JobSystemSetCategorySignal(EJobType type, Signal* sig)
{
	SAFE_DELETE(JobSystem::GetInstance()->signals[type]);
	JobSystem::GetInstance()->signals[type] = sig;
}

//------------------------------------------------------------------------
Job* JobCreate(EJobType type, job_work_cb work_cb, void *user_data)
{
	Job *job = new Job();
	job->type = type;
	job->work_cb = work_cb;
	job->user_data = user_data;
	job->num_dependencies = 1;

	return job;
}

//------------------------------------------------------------------------
void JobDispatchAndRelease(Job *job)
{
	// if I'm not ready to run, don't. 
	uint dcount = AtomicDecrement(&job->num_dependencies);
	if (dcount != 0) 
	{
		return;
	}

	JobSystem* js = JobSystem::GetInstance();
	js->queues[job->type].push(job);
	Signal *signal = js->signals[job->type];
	if (nullptr != signal) 
	{
		signal->signal_all();
	}
}
//------------------------------------------------------------------------
// THIS SHOULD BE MOVED TO A JOB CONSUMER OBJECT!
uint JobConsumeAll(EJobType type)
{
	Job *job;
	uint processed_jobs = 0;

	Thread::Queue<Job*> &queue = JobSystem::GetInstance()->queues[type];
	while (queue.pop(&job)) 
	{
		job->work_cb(job->user_data);
		++processed_jobs;

		job->OnFinish();
		delete job;
	}

	return processed_jobs;
}
////////////////////////////////////////////////////////////////////////// //////////////////////////////////////////////////////////////////////////
void JobConsumer::AddCategory(uint category)
{
	m_categories.push_back((EJobType)category);
}

bool JobConsumer::ConsumeJob()
{
	Job *job;

	for (uint i = 0; i < m_categories.size(); ++i)
	{
		Thread::Queue<Job*> &queue = JobSystem::GetInstance()->queues[m_categories[i]];

		if (queue.empty())
			continue;


		queue.pop(&job);
		job->work_cb(job->user_data);
		job->OnFinish();
		delete job;
		return true;
	}


	return false;
}

uint JobConsumer::ConsumeAll()
{
	Job *job;
	uint processed_jobs = 0;

	for (uint i = 0; i < m_categories.size(); ++i)
	{
		Thread::Queue<Job*> &queue = JobSystem::GetInstance()->queues[m_categories[i]];
		while (queue.pop(&job))
		{
			job->work_cb(job->user_data);
			++processed_jobs;

			job->OnFinish();
			delete job;
		}
	}
	

	return processed_jobs;
}
