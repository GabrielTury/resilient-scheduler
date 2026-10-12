#include "Commands.h"
#include <iostream>

void TestCommand::execute()
{
	std::cout << "Executing test command with option: " << option << std::endl;
}