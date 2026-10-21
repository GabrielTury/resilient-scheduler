#include "Policy.h"
#include <iostream>
#include <algorithm>

std::vector<Job> WorkPolicy::get_available_jobs(std::vector<Job>& jobs)
{
	std::vector<Job> availableJobs;
	for (Job& j : jobs)
	{
		if (j.get_memory() > maxMem)
		{
			j.declare_impossible();
			continue;
		}

		bool bUnassigned = j.get_active() && !j.is_ticking();

		if (bUnassigned)
		{
			availableJobs.emplace_back(j);
		}
	}

	return availableJobs;
}


void WorkPolicy::set_maxMem(const std::vector<Worker>& workers)
{
	for (const Worker& w : workers)
	{
		if (w.get_max_memory() > maxMem)
		{
			maxMem = w.get_max_memory();
		}
	}
}

void SJFPolicy::assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers)
{
	std::vector<Job> availableJobs = get_available_jobs(jobs);
	
	if (availableJobs.empty()) return;

	for (Worker& w : workers)
	{
		if (w.get_active() && w.get_working_jobId() == "")
		{
			auto it = std::min_element(availableJobs.begin(), availableJobs.end(), [&w](const Job& j, const Job& j2) {
				bool memFits = j.get_memory() <= w.get_max_memory();
				bool lesserDuration = j.get_duration() < j2.get_duration();
				return lesserDuration && memFits;
				});

			std::string assignId = it != availableJobs.end() ? it->get_id() : "";
			w.set_job(assignId);
			availableJobs.erase(it);
		}
	}
}

void FIFOPolicy::assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers)
{
	std::vector<Job> availableJobs = get_available_jobs(jobs);

	if (availableJobs.empty()) return;

	for (Worker& w : workers)
	{
		if (w.get_active() && w.get_working_jobId() == "")
		{
			auto it = std::min_element(availableJobs.begin(), availableJobs.end(), [&w](const Job& j, const Job& j2) {
				bool memFits = j.get_memory() <= w.get_max_memory();
				bool minId = std::stoi(j.get_id()) < std::stoi(j2.get_id());
				return minId && memFits;
				});

			std::string assignId = (it != availableJobs.end()) ? it->get_id() : "";
			w.set_job(assignId);
			availableJobs.erase(it);
		}
	}
}

void RRPolicy::assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers)
{
	const Tick quantumTicks = quantum == 0 ? 1 : quantum;
	std::vector<Job> availableJobs = get_available_jobs(jobs);

	for (Worker& w : workers)
	{
		if (!w.get_active() || w.get_working_jobId().empty()) continue;

		auto runningJob = std::find_if(jobs.begin(), jobs.end(), [&w](const Job& job) {
			return job.get_id() == w.get_working_jobId();
			});

		if (runningJob == jobs.end()) continue;

		const Tick elapsedTicks = runningJob->get_duration() - runningJob->get_remainingTicks();
		if (elapsedTicks > 0 && elapsedTicks % quantumTicks == 0)
		{
			runningJob->pause();
			availableJobs.emplace_back(*runningJob);
			w.free_worker();
		}
	}

	std::stable_sort(availableJobs.begin(), availableJobs.end(), [quantumTicks](const Job& left, const Job& right) {
		const Tick leftQuanta = (left.get_duration() - left.get_remainingTicks()) / quantumTicks;
		const Tick rightQuanta = (right.get_duration() - right.get_remainingTicks()) / quantumTicks;
		return leftQuanta < rightQuanta;
		});

	for (Worker& worker : workers)
	{
		if (!worker.get_active() || !worker.get_working_jobId().empty()) continue;

		auto nextJob = std::find_if(availableJobs.begin(), availableJobs.end(), [&worker](const Job& job) {
			return job.get_memory() <= worker.get_max_memory();
			});

		if (nextJob == availableJobs.end()) continue;

		worker.set_job(nextJob->get_id());
		availableJobs.erase(nextJob);
	}
}
