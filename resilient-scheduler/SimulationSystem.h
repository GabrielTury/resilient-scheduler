#pragma once
#include <vector>
#include "Job.h"
#include "Worker.h"
#include "Failure.h"
#include "Types.h"

class SimulationSystem
{
private:
	int tickCounter = 0;

	std::vector<Job> totalJobs;
	std::vector<Worker> totalWorkers;
	std::vector<Failure> totalFailures;

	std::vector<Job> activeJobs;
	std::vector<Worker> activeWorkers;

private:
	
public:
	SimulationSystem() {};	

	void Start();
	void Tick();

	void assign_vectors(std::vector<Job> jobs, std::vector<Worker> workers, std::vector<Failure> failures);	
};

