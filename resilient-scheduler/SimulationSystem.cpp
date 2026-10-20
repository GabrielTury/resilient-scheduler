#include "SimulationSystem.h"
#include <iostream>
#include <algorithm>
#include "SimulationBaseObject.h"

void SimulationSystem::assign_policy()
{
	switch (policy)
	{
		case SJF:
			activePolicy = std::make_unique<SJFPolicy>();
			break;
		default:
			break;
	}
}

void SimulationSystem::update_workers()
{
	for (Failure& f : m_failures)
	{
		f.update(tickCounter);

		//@todo remake this to avoid repetition and avoid looping every frame for optimizations
		if (f.get_active())
		{
			auto it = std::find_if(m_workers.begin(), m_workers.end(), [&f](const Worker& w) {
				return w.get_id() == f.get_workerId();
				});
			
			if (it != m_workers.end())
			{
				it->set_active(false); //disables worker @todo make worker take care of its own disabling
			}
		}
		else
		{
			auto it = std::find_if(m_workers.begin(), m_workers.end(), [&f](const Worker& w) {
				return w.get_id() == f.get_workerId();
				});

			if (it != m_workers.end())
			{
				it->set_active(true); //disables worker @todo make worker take care of its own disabling
			}
		}
	}
}

void SimulationSystem::update_jobs()
{	
	for (Job& j : m_Jobs)
	{
		j.update(tickCounter);
		j.get_remainingTicks() <= 0 ? completedJobs++ : false;
	}	
}

void SimulationSystem::do_work()
{
	for (Worker& w : m_workers)
	{
		if (!w.get_active()) continue;
		
		auto it = std::find_if(m_Jobs.begin(), m_Jobs.end(), [&w](const Job& j) {
			return j.get_id() == w.get_working_jobId();
		});
			
		if (it == m_Jobs.end()) continue;
			
		it->work(tickCounter);				
		if (it->get_remainingTicks() == 0) w.free_worker();					
	}
}

void SimulationSystem::create_simulation_objects()
{
	for (const JobData& j : srcJobs)
	{
		m_Jobs.emplace_back(Job(j));
	}

	for (const WorkerData& w : srcWorkers)
	{
		m_workers.emplace_back(Worker(w));
	}

	for (const FailureData& f : srcFailures)
	{
		m_failures.emplace_back(Failure(f));
	}

}

void SimulationSystem::Start()
{	
	create_simulation_objects();
	assign_policy();
	log = std::make_unique<Logger>();
	bool writeHeader = true;
	log->writeln_traces(m_Jobs, m_workers, writeHeader);
	Tick();
}

void SimulationSystem::Tick()
{
	do 
	{
		update_workers();
		update_jobs();
		activePolicy->assign_jobs(m_Jobs, m_workers);
		do_work();
		log->writeln_traces(m_Jobs, m_workers, tickCounter);
		std::cout << "Tick" << "\n";
		tickCounter++;
	} while (completedJobs <= m_Jobs.size());
	
}

void SimulationSystem::assign_data(std::vector<JobData> jobs, std::vector<WorkerData> workers, std::vector<FailureData> failures)
{
	srcJobs = std::move(jobs);
	srcWorkers = std::move(workers);
	srcFailures = std::move(failures);
}

void SimulationSystem::assign_logger(std::unique_ptr<Logger>& logger)
{
	log = std::move(logger);
}
