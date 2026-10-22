#include "Logger.h"
#include <fstream>
#include <vector>
#include <unordered_map>
#include "CsvUtils.hpp"
#include "json.hpp"

#define STRING(x) std::to_string(x)

std::string Logger::read_job_state(const JobStates& state)
{
	switch (state)
	{
		case JobStates::NOTARRIVED:
			return "not_arrived";
		case JobStates::COMPLETED:
			return "completed";
		case JobStates::LATE:
			return "late";
		case JobStates::WAITING:
			return "waiting";
		case JobStates::RUNNING:
			return "running";
	}
	return "UNDEFINED_STATE_ERROR";
}

void Logger::write_jobtrace_line(const std::vector<Job>& jobs, const std::vector<Worker>& workers, Tick tick, bool bWriteHeader)
{
	if (bWriteHeader)
	{
		CsvUtils::empty_file(jobTraceFile);
		std::vector<std::string> headers = { "tick", "job_id", "state_during_interval", "worker_id", "remaining_before", "remaining_after", "completion_tick" };
		CsvUtils::writeln_csv(headers, jobTraceFile);
		return;
	}

	std::unordered_map<std::string, std::string> workerIdMap;
	for (Worker w : workers)
	{
		std::string wJId = w.get_working_jobId() != "" ? w.get_working_jobId() : w.get_last_jobId();
		std::pair<std::string, std::string> pair(wJId, w.get_id());
		workerIdMap.insert(pair);
	}

	for (Job j : jobs)
	{
		std::vector<std::string> elements;
		elements.emplace_back(STRING(tick));

		elements.emplace_back(j.get_id());		
		elements.emplace_back(read_job_state(j.get_state()));
				
		auto found = workerIdMap.find(j.get_id());
		std::string wId = found == workerIdMap.end() ? "-" : found->second;
		elements.emplace_back(wId);

		std::string remaingTicks = STRING((int)j.get_remainingTicks());
		std::string remainingBefore = j.is_ticking() ? STRING(j.get_remainingTicks() + 1) : remaingTicks;//write function is called at the end of Tick
		elements.emplace_back(remainingBefore);
		elements.emplace_back(remaingTicks);

		std::string completionTick = (j.get_completion_tick() < 9999) ? STRING(j.get_completion_tick() + 1) : "-"; //@todo change to avoid 1000 ticks limit
		elements.emplace_back(completionTick);
		CsvUtils::writeln_csv(elements, jobTraceFile);
	}
}

void Logger::write_workertrace_line(const std::vector<Worker>& workers, Tick tick, bool bWriteHeader)
{
	if (bWriteHeader)
	{
		CsvUtils::empty_file(workerTraceFile);
		std::vector<std::string> headers = { "tick", "worker_id", "available", "job_id", "busy" };
		CsvUtils::writeln_csv(headers, workerTraceFile);
		return;
	}

	for (Worker w : workers)
	{
		std::vector<std::string> elements;
		elements.emplace_back(STRING(tick));
		elements.emplace_back(w.get_id());
		elements.emplace_back(w.get_active() ? "ON" : "OFF");
		elements.emplace_back(w.get_working_jobId() != "" ? w.get_working_jobId() : w.get_last_jobId()); //@todo 
		elements.emplace_back(w.get_working_jobId() != "" ? "busy" : "free");

		CsvUtils::writeln_csv(elements, workerTraceFile);
	}

	accumulate_worker_metrics(workers);
}

std::string Logger::policy_to_string(Policies p)
{
	switch (p)
	{
		case Policies::FIFO:  return "fifo";
		case Policies::SJF:   return "sjf";
		case Policies::RR:    return "round-robin";
		case Policies::LateLast: return "late-last";
	}
	return "unknown";
}

std::string Logger::failure_policy_to_string(RecoveryMode fp)
{
	switch (fp)
	{
		case RecoveryMode::RESUME:  return "resume";
		case RecoveryMode::RESTART: return "restart";
	}
	return "unknown";
}

void Logger::accumulate_worker_metrics(const std::vector<Worker>& workers)
{
	for (const Worker& w : workers)
	{
		if (w.get_active())
		{
			workerAvailableTicks[w.get_id()]++;
			if (!w.get_working_jobId().empty())
			{
				workerBusyTicks[w.get_id()]++;
			}
		}
	}
}

void Logger::set_filenames()
{
	 summaryFile = basePath + policy_to_string(policy) + "_" + failure_policy_to_string(recoveryMode) + "_" + "summary.json";
	 jobTraceFile = basePath + policy_to_string(policy) + "_" + failure_policy_to_string(recoveryMode) + "_" + "job_trace.csv";
	 workerTraceFile = basePath + policy_to_string(policy) + "_" + failure_policy_to_string(recoveryMode) + "_" + "worker_trace.csv";
}

void Logger::initialize_values(Policies& p, RecoveryMode& rec, Quantum q)
{
	set_policy(p, q);
	set_recovery_mode(rec);
	set_filenames();
}

void Logger::write_summary(const std::vector<Job>& jobs, const std::vector<Worker>& workers)
{
	using json = nlohmann::ordered_json;

	int completed = 0;
	int uncompleted = 0;
	Tick makespan = 0;
	int deadlineMisses = 0;

	json jobArray = json::array();

	for (const Job& j : jobs)
	{
		bool isCompleted = (j.get_state() == JobStates::COMPLETED);

		if (isCompleted)
		{
			completed++;
			Tick completionBoundary = j.get_completion_tick() + 1;
			if (completionBoundary > makespan) makespan = completionBoundary;

			bool deadlineMissed = completionBoundary > j.get_deadline();
			if (deadlineMissed) deadlineMisses++;

			json jobObj;
			jobObj["job_id"] = j.get_id();
			jobObj["completion_tick"] = completionBoundary;
			jobObj["waiting_ticks"] = j.get_waitingTicks();
			jobObj["turnaround_ticks"] = completionBoundary - j.get_arrival();
			jobObj["deadline_missed"] = deadlineMissed;
			jobArray.push_back(jobObj);
		}
		else
		{
			uncompleted++;

			json jobObj;
			jobObj["job_id"] = j.get_id();
			jobObj["completion_tick"] = nullptr;
			jobObj["waiting_ticks"] = j.get_waitingTicks();
			jobObj["turnaround_ticks"] = nullptr;
			jobObj["deadline_missed"] = nullptr;
			jobArray.push_back(jobObj);
		}
	}

	json workerArray = json::array();
	for (const Worker& w : workers)
	{
		Tick busy = workerBusyTicks[w.get_id()];
		Tick available = workerAvailableTicks[w.get_id()];
		double utilisation = (available > 0) ? static_cast<double>(busy) / available : 0.0;

		json workerObj;
		workerObj["worker_id"] = w.get_id();
		workerObj["busy_available_ticks"] = busy;
		workerObj["available_ticks"] = available;
		workerObj["utilisation"] = utilisation;
		workerArray.push_back(workerObj);
	}

	json summary;
	summary["policy"] = policy_to_string(policy);
	summary["quantum"] = quantum;
	summary["recovery"] = failure_policy_to_string(recoveryMode);
	summary["completed"] = completed;
	summary["uncompleted"] = uncompleted;
	summary["makespan"] = makespan;
	summary["observation_horizon"] = { 0, makespan };
	summary["deadline_misses"] = deadlineMisses;
	summary["jobs"] = jobArray;
	summary["workers"] = workerArray;

	std::ofstream outFile(summaryFile);
	if (outFile.is_open())
	{
		outFile << summary.dump(2);
		outFile.close();
	}
}

void Logger::writeln_traces(const std::vector<Job>& jobs, const std::vector<Worker>& workers, Tick tick, bool bWriteHeaders)
{

	write_jobtrace_line(jobs, workers, tick, bWriteHeaders);

	write_workertrace_line(workers, tick, bWriteHeaders);
}
