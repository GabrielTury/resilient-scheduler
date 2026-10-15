#pragma once
#include <string>
#include <tuple>
#include "Types.h"


class Job
{
private:
	std::string id;
	Tick arrival;
	Tick duration;
	int memoryUnits;
	int priority;
	Tick deadline;


public:
	using TypeTuple = std::tuple<std::string, Tick, Tick, int, int, Tick>;
	Job(std::string id, Tick arrival, Tick duration, int memoryUnits, int priority, Tick deadline)
		: id(id), arrival(arrival), duration(duration), memoryUnits(memoryUnits), priority(priority), deadline(deadline) {};

	Job(Job&&) noexcept = default;
	Job(const Job&) = default;
	Job& operator=(Job&&) noexcept = default;
};

