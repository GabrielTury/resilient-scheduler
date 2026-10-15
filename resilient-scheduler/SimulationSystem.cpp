#include "SimulationSystem.h"

void SimulationSystem::Tick()
{

	tickCounter++;
}

void SimulationSystem::assign_vectors(std::vector<Job> jobs, std::vector<Worker> workers, std::vector<Failure> failures)
{
	totalJobs = jobs;
	totalWorkers = workers;
	totalFailures = failures;
}
