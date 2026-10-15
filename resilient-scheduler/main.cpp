#include <iostream>
#include "Commands.h"
#include "Types.h"
#include "SimulationSystem.h"
#include "CsvUtils.hpp"
#include "Job.h"
#include <array>
#include <functional>

constexpr std::array<std::string_view, 5> commandList{
	"--test",
	"--base_folder",
	"--workers",
	"--jobs",
	"--failures"
};

constexpr std::array<int (Command::*)(const std::string&), commandList.size()> commandFuncs{
	&Command::test_command,
	&Command::base_folder_command,
	&Command::workers_command,
	&Command::jobs_command,
	&Command::failures_command
};

constexpr int find_command_index(const std::string_view cmd)
{
	for (int i = 0; std::string_view s : commandList)
	{
		if (s == cmd) return i;
		i++;
	}

	return INVALID_COMMAND;
}

int process_arguments(int argc, char** argv, SimulationSystem& sim)
{
	Command command;
	int errorCode = SUCCESS;
	int i = 1;

	while (errorCode == SUCCESS && i < argc)
	{
		std::string_view cmdStr(argv[i]);
		std::string optionStr = (i + 1 < argc) ? argv[i + 1] : "";
		int cmdIndex = find_command_index(cmdStr);

		if (cmdIndex != INVALID_COMMAND)
		{
			errorCode = (command.*commandFuncs[cmdIndex])(optionStr);
		}
		else
		{
			std::cerr << "Invalid command: " << cmdStr << std::endl;
			errorCode = INVALID_COMMAND;
		}

		i += 2;
	}	
	
	if(errorCode == SUCCESS)
		sim.assign_vectors(command.take_jobs(), command.take_workers(), command.take_failures());

	return errorCode;
}

int main(int argc, char** argv)
{		
	SimulationSystem simSys{};

	int res = process_arguments(argc, argv, simSys);

	simSys.Start();

	return res;
}