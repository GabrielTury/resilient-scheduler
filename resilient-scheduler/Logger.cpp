#include "Logger.h"
#include <fstream>
#include <vector>
#include <unordered_map>
#include "CsvUtils.hpp"

#define STRING(x) std::to_string(x)

std::string Logger::read_job_state(const JobStates& state)
{
	switch (state)
	{
		case JobStates::NOTARRIVED:
			return "NOTARRIVED";
		case JobStates::READY:
			return "READY";
		case JobStates::COMPLETED:
			return "COMPLETED";
		case JobStates::LATE:
			return "LATE";
		case JobStates::WAITING:
			return "WAITING";
	}
	return "UNKNOWN";
}

void Logger::write_jobtrace_line(const std::vector<Job>& jobs, const std::vector<Worker>& workers, Tick tick, bool bWriteHeader)
{
	if (bWriteHeader)
	{
		std::vector<std::string> headers = { "tick", "job_id", "state_during_interval", "worker_id", "remaining_before", "remaining_after", "completion_tick" };
		CsvUtils::writeln_csv(headers, jobTraceFile);
		return;
	}
	std::unordered_map<std::string, std::string> workerIdMap;
	for (Worker w : workers)
	{
		std::pair<std::string, std::string> pair(w.get_working_jobId(), w.get_id());
		workerIdMap.insert(pair);
	}

	for (Job j : jobs)
	{
		std::vector<std::string> elements;
		elements.emplace_back(STRING(tick));

		elements.emplace_back(j.get_id());		
		elements.emplace_back(read_job_state(j.get_state()));
				
		auto found = workerIdMap.find(j.get_id());
		std::string wId = found == workerIdMap.end() ? "empty" : found->second;
		elements.emplace_back(wId);

		std::string remaingTicks = STRING(j.get_remainingTicks());
		std::string remainingBefore = j.is_ticking() ? STRING(j.get_remainingTicks() + 1) : remaingTicks;//write function is called at the end of Tick
		elements.emplace_back(remainingBefore);
		elements.emplace_back(remaingTicks);

		elements.emplace_back(STRING(j.get_completion_tick()));
	}
	

}

void Logger::write_workertrace_line(const std::vector<Worker>& workers, Tick tick, bool bWriteHeader)
{
	if (bWriteHeader)
	{
		std::vector<std::string> headers = { "tick", "worker_id", "available", "job_id", "busy" };
		CsvUtils::writeln_csv(headers, jobTraceFile);
		return;
	}
}

void Logger::writeln_traces(const std::vector<Job>& jobs, const std::vector<Worker>& workers, Tick tick,bool bWriteHeaders)
{
	write_jobtrace_line(jobs, workers, bWriteHeaders);
	write_workertrace_line(workers, bWriteHeaders);
}
