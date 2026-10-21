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

	Policies chosenPolicy = Policies::SJF;
	RecoveryMode chosenRec = RecoveryMode::RESUME;

	Quantum quantum = 0;
public:
	std::vector<JobData> take_jobs();
	std::vector<WorkerData> take_workers();
	std::vector<FailureData> take_failures();
	std::string take_basePath();
	Policies take_policy();
	RecoveryMode take_rec_mode();
	Quantum get_quantum();
public:
	int test_command(const std::string& option);

	int base_folder_command(const std::string& option);

	int workers_command(const std::string& option);

	int jobs_command(const std::string& option);

	int failures_command(const std::string& option);

	int policy_command(const std::string& option);

	int recoverymode_command(const std::string& option);

	int quantum_command(const std::string& option);
};