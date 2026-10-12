#pragma once

#include <string>
#include <vector>

class Command {
protected:
	std::string label;
	std::string option;
	
	std::vector<std::string> validOptions;
public:
	Command(const std::string& inLabel, const std::string& inOption) : label(inLabel), option(inOption) {};	

	virtual void execute() = 0;
};

class TestCommand : public Command {
public:
	TestCommand(const std::string& inOption) : Command("test", inOption) {};

	void execute() override;

};