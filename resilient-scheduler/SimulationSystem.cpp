#include "SimulationSystem.h"
#include <iostream>

void SimulationSystem::Start()
{	
}

void SimulationSystem::Tick()
{
	std::cout << "Tick" << "\n";
	tickCounter++;
}

void SimulationSystem::assign_vectors(std::vector<Job> jobs, std::vector<Worker> workers, std::vector<Failure> failures)
{
	totalJobs = std::move(jobs);
	totalWorkers = std::move(workers);
	totalFailures = std::move(failures);
}
