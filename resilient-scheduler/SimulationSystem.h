#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "Job.h"
#include "Worker.h"
#include "Failure.h"
#include "Types.h"
#include "Policy.h"
#include "Logger.h"

class SimulationSystem
{
private:
	int tickCounter = 0;
	
	std::vector<JobData> srcJobs;
	std::vector<Job> m_Jobs;
	std::vector<std::string> failedJobIds;

	std::vector<WorkerData> srcWorkers;
	std::vector<Worker> m_workers;
	std::vector<Worker> availableWorkers;

	std::vector<FailureData> srcFailures;
	std::vector<Failure> m_failures;

	std::unique_ptr<WorkPolicy> activePolicy;
	std::unique_ptr<Logger> log;

	Quantum quantum = 0;
	Policies policy = Policies::SJF;
	RecoveryMode recMode = RecoveryMode::RESTART;

	int completedJobs = 0;
private:
	void assign_policy();

	//Updates failures and deactivates workers
	void update_workers();

	void update_jobs();

	void do_work();	

	void create_simulation_objects();

	void handle_job_failure(Job& job);

public:
	SimulationSystem() {};	

	void Start();
	void Tick();

	void assign_data(std::vector<JobData> jobs, std::vector<WorkerData> workers, std::vector<FailureData> failures, Policies pol ,Quantum quantum, RecoveryMode rec);	
	void assign_logger(std::unique_ptr<Logger>& logger);
};

