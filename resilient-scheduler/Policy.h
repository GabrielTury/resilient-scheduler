#pragma once
#include <vector>
#include <memory>
#include "Job.h"
#include "Failure.h"
#include "Worker.h"

class WorkPolicy
{
public:
	WorkPolicy() {};

	void handle_failures(const std::vector<Failure>& failure, const std::vector<Worker>& workers);

	//virtual void assign_jobs(const std::vector<Job>& jobs, const std::vector<Worker>& workers) = 0;
};

