#pragma once
#include <vector>
#include <memory>
#include <deque>
#include <unordered_map>
#include "Job.h"
#include "Failure.h"
#include "Worker.h"

class WorkPolicy
{
protected:
	int maxMem = 0;
public:
	virtual ~WorkPolicy() = default;

	virtual void assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers) = 0;

	void set_maxMem(const std::vector<Worker>& workers);
	std::vector<Job> get_available_jobs(std::vector<Job>& jobs);
};

class SJFPolicy : public WorkPolicy {
public:
	SJFPolicy() {};

	void assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers) override;
};

class FIFOPolicy : public WorkPolicy {
public:
	FIFOPolicy() {};

	void assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers) override;

};

class RRPolicy : public WorkPolicy {
private:
	Quantum quantum = 1;
private:

public:
	RRPolicy() {};
	void assign_quantum(const Quantum quant) { quantum = quant; };
	void assign_jobs(std::vector<Job>& jobs, std::vector<Worker>& workers) override;
};

