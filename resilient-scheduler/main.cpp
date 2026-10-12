#include <iostream>
#include "Commands.h"

int main(int argc, char** argv)
{
	if (argc > 1)
	{
		TestCommand command{ argv[1] };
		command.execute();
	}
		
	return 0;
}