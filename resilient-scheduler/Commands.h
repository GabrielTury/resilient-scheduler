#pragma once

#include <string>
#include "Job.h"
#include "Failure.h"
#include "Worker.h"
#include <vector>

class Command {
private:
	std::string basePath;

	std::vector<JobData> parsedJobs;
	std::vector<WorkerData> parsedWorkers;
	std::vector<FailureData> parsedFailures;
public:
	std::vector<JobData> take_jobs();
	std::vector<WorkerData> take_workers();
	std::vector<FailureData> take_failures();

public:
	int test_command(const std::string& option);

	int base_folder_command(const std::string& option);

	int workers_command(const std::string& option);

	int jobs_command(const std::string& option);

	int failures_command(const std::string& option);
};