#include "SimulationSystem.h"
#include <iostream>
#include <algorithm>
#include "SimulationBaseObject.h"

void SimulationSystem::assign_policy()
{
	switch (policy)
	{
		case Policies::SJF:
			activePolicy = std::make_unique<SJFPolicy>();
			break;
		case Policies::FIFO:
			activePolicy = std::make_unique<FIFOPolicy>();
			break;
		case Policies::RR:
			activePolicy = std::make_unique<RRPolicy>();
			static_cast<RRPolicy*>(activePolicy.get())->assign_quantum(quantum);
			break;
		case Policies::LateLast:
			activePolicy = std::make_unique<LateLast>();
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
	}

	for (Worker& worker : m_workers)
	{
		const bool hasActiveFailure = std::any_of(m_failures.begin(), m_failures.end(), [&worker](const Failure& failure) {
			return (failure.get_workerId() == worker.get_id()) && failure.get_active();
		});

		if (hasActiveFailure)
		{
			if (!worker.get_active()) continue;

			const std::string failedJobId = worker.get_working_jobId();
			worker.set_active(false);
			if (!failedJobId.empty()) failedJobIds.emplace_back(failedJobId);
		}
		else if (!worker.get_active())
		{
			worker.set_active(true);
		}
	}
}

void SimulationSystem::update_jobs()
{	
	for (Job& j : m_Jobs)
	{		
		handle_job_failure(j);
		j.update(tickCounter);

		bool bImpossible = j.get_state() == JobStates::IMPOSSIBLE;
		bool bCompleted = j.get_state() == JobStates::COMPLETED;

		if ((bCompleted || bImpossible) && !j.is_finished())
		{
			j.finish();
			completedJobs++;
		}	
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

void SimulationSystem::handle_job_failure(Job& job)
{
	auto it = std::find_if(failedJobIds.begin(), failedJobIds.end(), [&job](const std::string& s) {
		return job.get_id() == s;
		});

	if (it == failedJobIds.end()) return;

	switch (recMode)
	{
	case RecoveryMode::RESUME:
		job.pause();
		break;
	case RecoveryMode::RESTART:
		job.restart();
		break;
	}

	failedJobIds.erase(it);
}

void SimulationSystem::free_workers()
{
	for (Worker& worker : m_workers)
	{
		auto completedJob = std::find_if(m_Jobs.begin(), m_Jobs.end(), [&worker](const Job& job) {
			return job.get_id() == worker.get_working_jobId() && job.get_state() == JobStates::COMPLETED;
			});
		if (completedJob != m_Jobs.end()) worker.free_worker();
	}
}

void SimulationSystem::Start()
{	
	create_simulation_objects();

	assign_policy();
	activePolicy->set_maxMem(m_workers);

	log->initialize_values(policy, recMode, quantum);	
	bool writeHeader = true;
	log->writeln_traces(m_Jobs, m_workers, 0, writeHeader);

	Tick();
}

void SimulationSystem::Tick()
{
	do 
	{
		update_workers();
		update_jobs();
		if (completedJobs == m_Jobs.size()) continue;

		activePolicy->assign_jobs(m_Jobs, m_workers);
		do_work();
		log->writeln_traces(m_Jobs, m_workers, tickCounter, false);

		free_workers();

		std::cout << "Tick " << tickCounter << "\n";
		tickCounter++;
		if (tickCounter > 999)
		{
			std::cerr << "Error: Inifinte Loop" << std::endl;
			break;
		}
	} while (completedJobs < m_Jobs.size());

	log->write_summary(m_Jobs, m_workers);
}

void SimulationSystem::assign_data(std::vector<JobData> jobs, std::vector<WorkerData> workers, std::vector<FailureData> failures, Policies pol, Quantum quantum, RecoveryMode rec)
{
	srcJobs = std::move(jobs);
	srcWorkers = std::move(workers);
	srcFailures = std::move(failures);
	policy = std::move(pol);
	recMode = std::move(rec);
	this->quantum = quantum;
}

void SimulationSystem::assign_logger(std::unique_ptr<::Logger>& logger)
{
	log = std::move(logger);
}
