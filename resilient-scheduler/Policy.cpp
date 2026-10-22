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
			auto it = availableJobs.end();
			for (auto candidate = availableJobs.begin(); candidate != availableJobs.end(); ++candidate)
			{
				if (candidate->get_memory() > w.get_max_memory()) continue;
				if (it == availableJobs.end() || candidate->get_duration() < it->get_duration()) it = candidate;
			}

			if (it == availableJobs.end()) continue;
			w.set_job(it->get_id());
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
			auto it = availableJobs.end();
			for (auto candidate = availableJobs.begin(); candidate != availableJobs.end(); ++candidate)
			{
				if (candidate->get_memory() > w.get_max_memory()) continue;
				if (it == availableJobs.end() || std::stoi(candidate->get_id()) < std::stoi(it->get_id())) it = candidate;
			}

			if (it == availableJobs.end()) continue;
			w.set_job(it->get_id());
			availableJobs.erase(it);
		}
	}
}

bool RRPolicy::is_available(const std::vector<Job>& availableJobs, const std::string& jobId) const
{
	return std::any_of(availableJobs.begin(), availableJobs.end(), [&jobId](const Job& job) {
		return job.get_id() == jobId;
		});
}

void RRPolicy::assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers)
{
	const Tick quantumTicks = quantum == 0 ? 1 : quantum;

	std::vector<Job> availableJobs = get_available_jobs(jobs);	

	readyQueue.erase(std::remove_if(readyQueue.begin(), readyQueue.end(), [this, &availableJobs](const std::string& jobId) {
		return !this->is_available(availableJobs, jobId);
		}), readyQueue.end());

	for (auto progress = quantumProgress.begin(); progress != quantumProgress.end();)
	{
		const bool isRunning = std::any_of(workers.begin(), workers.end(), [&progress](const Worker& worker) {
			return worker.get_active() && worker.get_working_jobId() == progress->first;
		});

		if (!isRunning)
		{
			progress = quantumProgress.erase(progress);
		}
		else
		{
			++progress;
		}
	}

	for (Worker& w : workers)
	{
		if (!w.get_active() || w.get_working_jobId().empty()) continue;

		auto runningJob = std::find_if(jobs.begin(), jobs.end(), [&w](const Job& job) {
			return job.get_id() == w.get_working_jobId();
			});

		if (runningJob == jobs.end()) continue;

		const std::string jobId = runningJob->get_id();
		if (++quantumProgress[jobId] >= quantumTicks)
		{
			runningJob->pause();
			availableJobs.emplace_back(*runningJob);
			w.free_worker();
			quantumProgress.erase(jobId);
			readyQueue.emplace_back(jobId);
		}
	}

	for (const Job& job : availableJobs)
	{
		if (std::find(readyQueue.begin(), readyQueue.end(), job.get_id()) == readyQueue.end())
		{
			readyQueue.emplace_back(job.get_id());
		}
	}

	for (Worker& worker : workers)
	{
		if (!worker.get_active() || !worker.get_working_jobId().empty()) continue;

		auto nextJob = std::find_if(readyQueue.begin(), readyQueue.end(), [&availableJobs, &worker](const std::string& jobId) {
			auto job = std::find_if(availableJobs.begin(), availableJobs.end(), [&jobId](const Job& availableJob) {
				return availableJob.get_id() == jobId;
			});
			return job != availableJobs.end() && job->get_memory() <= worker.get_max_memory();
			});

		if (nextJob == readyQueue.end()) continue;

		worker.set_job(*nextJob);
		quantumProgress[*nextJob] = 0;
		readyQueue.erase(nextJob);
	}
}

void LateLast::assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers)
{
	std::vector<Job> availableJobs = get_available_jobs(jobs);

	for (Worker& w : workers)
	{
		if (!w.get_active() || w.get_working_jobId().empty()) continue;

		auto it = std::find_if(jobs.begin(), jobs.end(), [&w](const Job& job) {
			return job.get_id() == w.get_working_jobId();
		});

		if ((it == jobs.end()) || (it->get_state() != JobStates::LATE)) continue;

		const bool onTimeJobFits = std::any_of(availableJobs.begin(), availableJobs.end(), [&w](const Job& job) {
			return (job.get_state() != JobStates::LATE) && (job.get_memory() <= w.get_max_memory());
		});

		if (onTimeJobFits)
		{
			it->pause();
			w.free_worker();
			availableJobs.emplace_back(*it);
		}
	}

	std::stable_sort(availableJobs.begin(), availableJobs.end(), [](const Job& left, const Job& right) {
		const bool leftIsLate = left.get_state() == JobStates::LATE;
		const bool rightIsLate = right.get_state() == JobStates::LATE;

		if (leftIsLate != rightIsLate) return !leftIsLate;
		return left.get_priority() > right.get_priority();
	});

	for (Worker& worker : workers)
	{
		if (!worker.get_active() || (!worker.get_working_jobId().empty())) continue;

		auto nextJob = std::find_if(availableJobs.begin(), availableJobs.end(), [&worker](const Job& job) {
			return job.get_memory() <= worker.get_max_memory();
		});

		if (nextJob == availableJobs.end()) continue;

		worker.set_job(nextJob->get_id());
		availableJobs.erase(nextJob);
	}
}
