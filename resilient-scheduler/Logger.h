#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Job.h"
#include "Worker.h"
#include "Types.h"

class Logger
{
private:
	std::string basePath;
	std::string summaryFile = "unset_summary.json";
	std::string jobTraceFile = "unset_jobtrace.csv";
	std::string workerTraceFile = "unset_workertrace.csv";

	Policies policy = Policies::SJF;
	RecoveryMode recoveryMode = RecoveryMode::RESTART;

	std::unordered_map<std::string, Tick> workerBusyTicks;
	std::unordered_map<std::string, Tick> workerAvailableTicks;

private:
	std::string read_job_state(const JobStates& state);
	std::string policy_to_string(Policies p);
	std::string failure_policy_to_string(RecoveryMode fp);

	void write_jobtrace_line(const std::vector<Job>& jobs, const std::vector<Worker>& workers, Tick tick, bool bWriteHeader = false);
	void write_workertrace_line(const std::vector<Worker>& workers, Tick tick, bool bWriteHeader = false);

	void accumulate_worker_metrics(const std::vector<Worker>& workers);

	void set_policy(Policies p) { policy = p; };
	void set_recovery_mode(RecoveryMode rec) { recoveryMode = rec; };
	void set_filenames();

public:
	Logger(std::string bPath, std::string sumFile, std::string tFile, std::string wFile) : 
		basePath(bPath), summaryFile(sumFile), jobTraceFile(tFile), workerTraceFile(wFile) {};
	
	Logger(std::string bPath) : basePath(bPath) {};

	Logger() = default;

	Logger(Logger&&) noexcept = default;
	Logger(const Logger&) = default;
	Logger& operator=(Logger&&) noexcept = default;

	void initialize_values(Policies& p, RecoveryMode& rec);

	void write_summary(const std::vector<Job>& jobs, const std::vector<Worker>& workers);
	void writeln_traces(const std::vector<Job>& jobs, const std::vector<Worker>& workers, Tick tick = 0, bool bWriteHeaders = false);
};

