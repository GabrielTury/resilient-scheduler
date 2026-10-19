#pragma once
#include <string>
#include <tuple>
#include "SimulationBaseObject.h"

struct JobData {
	std::string id;
	Tick arrival;
	Tick duration;
	int memoryUnits;
	int priority;
	Tick deadline;

	using TypeTuple = std::tuple<std::string, Tick, Tick, int, int, Tick>;

	JobData(std::string id, Tick arrival, Tick duration, int memoryUnits, int priority, Tick deadline)
		: id(id), arrival(arrival), duration(duration), memoryUnits(memoryUnits), priority(priority), deadline(deadline) {
	};

	JobData(JobData&&) noexcept = default;
	JobData(const JobData&) = default;
	JobData& operator=(JobData&&) noexcept = default;
};

class Job : public SimulationBaseObject
{
private:
	bool bTicking{ false };

	std::string id;
	Tick arrival;
	Tick duration;
	int memoryUnits;
	int priority;
	Tick deadline;

	Tick remainingTicks;
public:
	Job() = default;

	Job(const JobData& jd)
		: bTicking(false),
		id(jd.id),
		arrival(jd.arrival),
		duration(jd.duration),
		memoryUnits(jd.memoryUnits),
		priority(jd.priority),
		deadline(jd.deadline),
		remainingTicks(jd.duration)
	{};

	void update(Tick currentTick) override;

	const Tick get_duration() const { return duration; };
	const std::string& get_id() const { return id; };
	const Tick get_remainingTicks() const { return remainingTicks; };
	const bool is_ticking() const { return bTicking; };
	const int get_memory() const { return memoryUnits; };
	void work();
};

