#include "Policy.h"
#include <iostream>

void SJFPolicy::assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers)
{
	std::vector<Job> availableJobs;
	for (Job& j : jobs)
	{
		bool bUnassigned = j.get_active() && !j.is_ticking();
		//bool bCompleted = j.get_remainingTicks() <= 0;

		if (bUnassigned)
		{
			availableJobs.emplace_back(j);
		}
	}

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

	std::cout << "Called Assign Job" << "\n";
}
