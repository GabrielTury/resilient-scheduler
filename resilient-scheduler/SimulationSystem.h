#pragma once
#include <vector>
#include "Job.h"
#include "Worker.h"
#include "Failure.h"

class SimulationSystem
{
private:
	int tickCounter = 0;

	std::vector<Job> totalJobs;
	std::vector<Worker> totalWorkers;
	std::vector<Failure> totalFailures;

	std::vector<Job> activeJobs;
public:
	SimulationSystem() {};
	SimulationSystem(std::vector<Job> inJobs, std::vector<Worker> inWorkers, std::vector<Failure> inFailures) : totalJobs(inJobs), totalWorkers(inWorkers), totalFailures(inFailures) {};


	void Tick();

	void assign_vectors(std::vector<Job> jobs, std::vector<Worker> workers, std::vector<Failure> failures);
};

