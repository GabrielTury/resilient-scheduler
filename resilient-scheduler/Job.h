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
	bool bFinished{ false }; //Means that there will be no more processing on this obj
	std::string id;
	Tick arrival;
	Tick duration;
	int memoryUnits;
	int priority;
	Tick deadline;

	Tick remainingTicks;
	Tick completionTick = 99999; //Negative while not completed
	Tick waitingTicks = 0;

	JobStates state = JobStates::NOTARRIVED;


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
	const int get_priority() const { return priority; };
	const JobStates& get_state() const { return state; };
	const Tick get_completion_tick() const { return completionTick; };
	const bool is_finished() const { return bFinished; };
	const Tick get_arrival() const { return arrival; };
	const Tick get_deadline() const { return deadline; };
	const Tick get_waitingTicks() const { return waitingTicks; };

	void work(Tick tick);
	void restart();
	void finish();
	void declare_impossible();
	void pause();
};

