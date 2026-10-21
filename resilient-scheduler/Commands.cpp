#include "Commands.h"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include "Types.h"
#include "CsvUtils.hpp"

std::vector<JobData> Command::take_jobs()
{
	if(parsedJobs.size() > 0)
		return std::move(parsedJobs);

	return std::vector<JobData>();
}

std::vector<WorkerData> Command::take_workers()
{
	if (parsedWorkers.size() > 0)
		return std::move(parsedWorkers);

	return std::vector<WorkerData>();
}

std::vector<FailureData> Command::take_failures()
{
	if (parsedFailures.size() > 0)
		return std::move(parsedFailures);

	return std::vector<FailureData>();
}

std::string Command::take_basePath()
{
	return std::move(basePath);
}

Policies Command::take_policy()
{
	return std::move(chosenPolicy);
}

RecoveryMode Command::take_rec_mode()
{
	return std::move(chosenRec);
}

Quantum Command::get_quantum()
{
	return quantum;
}

int Command::test_command(const std::string& option)
{
	std::cout << "Executing test command with option: " << option << std::endl;
	return SUCCESS;
}

int Command::base_folder_command(const std::string& option)
{
	std::error_code ec;
	if (!std::filesystem::is_directory(option, ec))
	{
		std::cerr << "Invalid base folder path: " << option << "\n";
		return INVALID_OPTION;
	}

	basePath = option;
	return SUCCESS;
}

int Command::workers_command(const std::string& option)
{
	std::string file = basePath + option;	

	if (!std::filesystem::exists(file))
	{
		std::cerr << "Workers file does not exist on: " << file << "\n";
		return INVALID_OPTION;
	}

	parsedWorkers = CsvUtils::parse<WorkerData>(file);
	return SUCCESS;
}

int Command::jobs_command(const std::string& option)
{
	std::string file = basePath + option;

	if (!std::filesystem::exists(file))
	{
		std::cerr << "Jobs file does not exist on: " << file << "\n";
		return INVALID_OPTION;
	}


	parsedJobs = CsvUtils::parse<JobData>(file);
	return SUCCESS;
}

int Command::failures_command(const std::string& option)
{
	std::string file = basePath + option;

	if (!std::filesystem::exists(file))
	{
		std::cerr << "Failuers file does not exist on: " << file << "\n";
		return INVALID_OPTION;
	}

	parsedFailures = CsvUtils::parse<FailureData>(file);
	return SUCCESS;
}

int Command::policy_command(const std::string& option)
{
	std::unordered_map<std::string, Policies> validOpts{ 
		{"SJF", Policies::SJF},
		{"RR", Policies::RR},
		{"FIFO", Policies::FIFO},
		{"SCORE", Policies::SCORE}
	};

	auto it = validOpts.find(option);

	if (it == validOpts.end()) return INVALID_OPTION;
	
	chosenPolicy = it->second;
	return SUCCESS;
	
}

int Command::recoverymode_command(const std::string& option)
{
	std::unordered_map<std::string, RecoveryMode> opts{
		{"RESUME", RecoveryMode::RESUME},
		{"RESTART", RecoveryMode::RESTART}
	};

	auto it = opts.find(option);

	if (it == opts.end()) return INVALID_OPTION;

	chosenRec = it->second;
	return SUCCESS;
}

int Command::quantum_command(const std::string& option)
{
	int quant = 0;
	auto [ptr, ec] = std::from_chars(option.data(), option.data() + option.size(), quant);

	if ((ec != std::errc()) || (ptr != option.data() + option.size())) {
		return INVALID_OPTION;
	}

	if (quant < 0) {
		return INVALID_OPTION;
	}

	quantum = quant;

	return SUCCESS;
}


