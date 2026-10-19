#pragma once
#include <vector>
#include <memory>
#include "Job.h"
#include "Failure.h"
#include "Worker.h"

class WorkPolicy
{
public:
	virtual ~WorkPolicy() = default;

	virtual void assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers) = 0;
};

class SJFPolicy : public WorkPolicy {
public:
	SJFPolicy() {};

	void assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers) override;
};

