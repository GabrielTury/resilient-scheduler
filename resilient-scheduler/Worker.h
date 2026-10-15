#pragma once
#include <string>
#include <tuple>

class Worker
{
private:
	std::string id;
	int memoryCapacity;


public:
	using TypeTuple = std::tuple<std::string, int>;
	Worker(std::string id, int memoryCapacity)
		: id(id), memoryCapacity(memoryCapacity) {};

	Worker(Worker&&) noexcept = default;
	Worker(const Worker&) = default;
	Worker& operator=(Worker&&) noexcept = default;
};

