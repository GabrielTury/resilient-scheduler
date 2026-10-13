#include "Commands.h"
#include <iostream>

void Command::test_command(const std::string& option)
{
	std::cout << "Executing test command with option: " << option << std::endl;
}
