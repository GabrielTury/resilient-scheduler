#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "Job.h"
#include "Worker.h"
#include "Failure.h"
#include "Types.h"
#include "Policy.h"

class SimulationSystem
{
private:
	int tickCounter = 0;
	//@todo add data so it has an imutable data source, and have other container for the jobs/workers/failures that will actually change
	std::vector<JobData> srcJobs;
	std::vector<Job> m_Jobs;

	std::vector<WorkerData> srcWorkers;
	std::vector<Worker> m_workers;
	std::vector<Worker> availableWorkers;

	std::vector<FailureData> srcFailures;
	std::vector<Failure> m_failures;

	std::unique_ptr<WorkPolicy> activePolicy;

	Policies policy = SJF;
	int completedJobs = 0;
private:
	void assign_policy();

	//Updates failures and deactivates workers
	void update_workers();

	void update_jobs();

	void do_work();

	void create_simulation_objects();
public:
	SimulationSystem() {};	

	void Start();
	void Tick();

	void assign_data(std::vector<JobData> jobs, std::vector<WorkerData> workers, std::vector<FailureData> failures);	
};

