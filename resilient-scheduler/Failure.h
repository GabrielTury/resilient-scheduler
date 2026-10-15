#pragma once
#include <string>
#include "Types.h"
#include <tuple>


class Failure
{
private:
	std::string id;
	Tick start;
	Tick duration;


public:
	using TypeTuple = std::tuple<std::string, Tick, Tick>;	
	Failure(std::string id, Tick start, Tick duration)
		: id(id), start(start), duration(duration) {};
};

