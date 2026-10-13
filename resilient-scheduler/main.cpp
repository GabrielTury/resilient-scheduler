#include <iostream>
#include "Commands.h"
#include "Types.h"
#include <array>
#include <functional>

constexpr std::array<std::string_view, 1> commandList{
	"--test",
};

constexpr std::array<void (Command::*)(const std::string&), commandList.size()> commandFuncs{
	&Command::test_command,
};

constexpr int findCommandIndex(const std::string_view cmd)
{
	for (int i = 0; std::string_view s : commandList)
	{
		if (s == cmd) return i;
		i++;
	}

	return INVALID_COMMAND;
}

void processArguments(int argc, char** argv)
{
	Command command;
	for (int i = 1; i < argc; i += 2)
	{
		std::string_view cmdStr(argv[i]);
		std::string optionStr = (i + 1 < argc) ? argv[i + 1] : "";
		int cmdIndex = findCommandIndex(cmdStr);
		if (cmdIndex != -1)
		{
			(command.*commandFuncs[cmdIndex])(optionStr);
		}
		else
		{
			std::cerr << "Invalid command: " << cmdStr << std::endl;
		}
	}
}

int main(int argc, char** argv)
{		
	processArguments(argc, argv);

	return SUCCESS;
}