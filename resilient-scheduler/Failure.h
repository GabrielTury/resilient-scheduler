#pragma once
#include <string>
#include "SimulationBaseObject.h"
#include <tuple>


struct FailureData {
	std::string workerId;
	Tick start;
	Tick duration;

	using TypeTuple = std::tuple<std::string, Tick, Tick>;

	FailureData(std::string id, Tick start, Tick duration)
		: workerId(id), start(start), duration(duration) {
	};

	FailureData(FailureData&&) noexcept = default;
	FailureData(const FailureData&) = default;
	FailureData& operator=(FailureData&&) noexcept = default;
};

class Failure : public SimulationBaseObject
{
private:
	std::string workerId;
	Tick start;
	Tick duration;
public:
	using TypeTuple = std::tuple<std::string, Tick, Tick>;	
	Failure(std::string id, Tick start, Tick duration)
		: workerId(id), start(start), duration(duration) {};

	Failure(const FailureData& fd) {
		workerId = fd.workerId;
		start = fd.start;
		duration = fd.duration;
	};

	void update(Tick tick) override;

	const std::string& get_workerId() const { return workerId; };
};


