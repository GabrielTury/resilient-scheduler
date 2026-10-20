#pragma once
#include <string>
#include <vector>
#include "Job.h"
#include "Worker.h"
#include "Types.h"

class Logger
{
private:
	std::string basePath;
	std::string summaryFile = basePath + "summary.json";
	std::string jobTraceFile = basePath + "job_trace.csv";
	std::string workerTraceFile = basePath + "worker_trace.csv";

	//std::string trace;
	//std::string summary;
private:
	std::string read_job_state(const JobStates& state);

	void write_jobtrace_line(const std::vector<Job>& jobs, const std::vector<Worker>& workers, Tick tick, bool bWriteHeader = false);
	void write_workertrace_line(const std::vector<Worker>& workers, Tick tick, bool bWriteHeader = false);

public:
	Logger(std::string bPath, std::string sumFile, std::string tFile, std::string wFile) : 
		basePath(bPath), summaryFile(sumFile), jobTraceFile(tFile), workerTraceFile(wFile) {};
	
	Logger() = default;

	void write_summary();
	void writeln_traces(const std::vector<Job>& jobs, const std::vector<Worker>& workers, Tick tick = 0, bool bWriteHeaders = false);
};

