#pragma once

#include <string>
#include "Job.h"
#include "Failure.h"
#include "Worker.h"
#include <vector>

class Command {
private:
	std::string basePath;

	std::vector<Job> parsedJobs;
	std::vector<Worker> parsedWorkers;
	std::vector<Failure> parsedFailures;
public:
	std::vector<Job> take_jobs();
	std::vector<Worker> take_workers();
	std::vector<Failure> take_failures();

public:
	int test_command(const std::string& option);

	int base_folder_command(const std::string& option);

	int workers_command(const std::string& option);

	int jobs_command(const std::string& option);

	int failures_command(const std::string& option);
};