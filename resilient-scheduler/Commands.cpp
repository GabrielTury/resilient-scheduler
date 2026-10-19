#include "Commands.h"
#include <iostream>
#include <filesystem>
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


