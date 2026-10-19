#pragma once
#include <string>
#include <tuple>
#include "SimulationBaseObject.h"

struct WorkerData {
	std::string id;
	int memoryCapacity;

	using TypeTuple = std::tuple<std::string, int>;

	WorkerData(std::string id, int memoryCapacity)
		: id(id), memoryCapacity(memoryCapacity) {
	};

	WorkerData(WorkerData&&) noexcept = default;
	WorkerData(const WorkerData&) = default;
	WorkerData& operator=(WorkerData&&) noexcept = default;
};

class Worker : public SimulationBaseObject
{
private:
	std::string workingJobId = "";
	std::string id;
	int memoryCapacity;

public:
	using TypeTuple = std::tuple<std::string, int>;
	Worker(std::string id, int memoryCapacity)
		: id(id), memoryCapacity(memoryCapacity) {};

	Worker(const WorkerData& wd) {
		id = wd.id;
		memoryCapacity = wd.memoryCapacity;
		set_active(true);
		};

public:
	const std::string& get_id() const { return id; };
	
	void set_active(const bool active) { bActive = active; };

	void set_job(const std::string& jobId) { workingJobId = jobId; };
	void free_worker() { workingJobId = ""; };
	const std::string get_working_jobId() const { return workingJobId; };
	const int get_max_memory() const { return memoryCapacity; };

	void update(Tick tick) override;
};


