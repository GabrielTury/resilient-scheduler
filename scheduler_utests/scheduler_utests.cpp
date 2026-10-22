#include "pch.h"
#include "CppUnitTest.h"
#include "Policy.h"
#include "SimulationSystem.h"
#include "json.hpp"
#include "Logger.h"

#include <cstdio>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	Job create_job(const std::string& id, Tick arrival, Tick duration, int memory, int priority = 1, Tick deadline = 10)
	{
		return Job(JobData(id, arrival, duration, memory, priority, deadline));
	}

	Worker create_worker(const std::string& id, int memory)
	{
		return Worker(WorkerData(id, memory));
	}

	void make_jobs_available(std::vector<Job>& jobs, Tick tick = 0)
	{
		for (Job& job : jobs)
		{
			job.update(tick);
		}
	}

	Tick run_recovery_scenario(RecoveryMode recoveryMode)
	{
		std::vector<JobData> jobs;
		jobs.emplace_back("001", 0, 4, 1, 1, 20);
		std::vector<WorkerData> workers;
		workers.emplace_back("001", 1);
		std::vector<FailureData> failures;
		failures.emplace_back("001", 1, 1);

		SimulationSystem simulation;
		simulation.assign_data(std::move(jobs), std::move(workers), std::move(failures), Policies::SJF, 0, recoveryMode);
		std::unique_ptr<::Logger> logger = std::make_unique<::Logger>("recovery_unit_test_");
		simulation.assign_logger(logger);
		simulation.Start();

		const std::string recoveryName = recoveryMode == RecoveryMode::RESUME ? "resume" : "restart";
		const std::string baseName = "recovery_unit_test_sjf_" + recoveryName + "_";
		const std::string summaryPath = baseName + "summary.json";
		std::ifstream summaryFile(summaryPath);
		if (!summaryFile)
		{
			throw std::runtime_error("Recovery simulation summary was not created: " + summaryPath);
		}

		const nlohmann::json summary = nlohmann::json::parse(summaryFile);
		const Tick completionTick = summary.at("jobs").at(0).at("completion_tick").get<Tick>();
		summaryFile.close();

		std::remove(summaryPath.c_str());
		std::remove((baseName + "job_trace.csv").c_str());
		std::remove((baseName + "worker_trace.csv").c_str());

		return completionTick;
	}
}

namespace schedulerutests
{
	TEST_CLASS(schedulerutests)
	{
	public:
		TEST_METHOD(SJF_SelectsShortestJobThatFitsEachWorker)
		{
			std::vector<Job> jobs{
				create_job("001", 0, 10, 8),
				create_job("002", 0, 6, 4),
				create_job("003", 0, 2, 4)
			};
			std::vector<Worker> workers{
				create_worker("002", 4),
				create_worker("001", 8)
			};
			make_jobs_available(jobs);

			SJFPolicy policy;
			policy.set_maxMem(workers);
			policy.assign_jobs(jobs, workers);

			Assert::IsTrue(workers[0].get_working_jobId() == "003");
			Assert::IsTrue(workers[1].get_working_jobId() == "002");
		}

		TEST_METHOD(FIFO_SelectsLowestNumericJobIdThatFitsEachWorker)
		{
			std::vector<Job> jobs{
				create_job("003", 0, 1, 2),
				create_job("002", 0, 10, 2),
				create_job("001", 0, 20, 8)
			};
			std::vector<Worker> workers{
				create_worker("002", 2),
				create_worker("001", 8)
			};
			make_jobs_available(jobs);

			FIFOPolicy policy;
			policy.set_maxMem(workers);
			policy.assign_jobs(jobs, workers);

			Assert::IsTrue(workers[0].get_working_jobId() == "002");
			Assert::IsTrue(workers[1].get_working_jobId() == "001");
		}

		TEST_METHOD(RR_RotatesJobsAfterTheirQuantumExpires)
		{
			std::vector<Job> jobs{
				create_job("001", 0, 10, 1),
				create_job("002", 0, 10, 1)
			};
			std::vector<Worker> workers{ create_worker("001", 4) };
			make_jobs_available(jobs);

			RRPolicy policy;
			policy.assign_quantum(1);
			policy.set_maxMem(workers);
			policy.assign_jobs(jobs, workers);
			Assert::IsTrue(workers[0].get_working_jobId() == "001");

			jobs[0].work(0);
			policy.assign_jobs(jobs, workers);
			Assert::IsTrue(workers[0].get_working_jobId() == "002");

			jobs[1].work(1);
			policy.assign_jobs(jobs, workers);
			Assert::IsTrue(workers[0].get_working_jobId() == "001");
		}

		TEST_METHOD(LateLast_PrioritizesOnTimeJobsBeforeLateJobs)
		{
			std::vector<Job> jobs{
				create_job("001", 0, 5, 1, 5, 0),
				create_job("002", 0, 5, 1, 1, 10),
				create_job("003", 0, 5, 1, 3, 10)
			};
			std::vector<Worker> workers{
				create_worker("001", 4),
				create_worker("002", 4),
				create_worker("003", 4)
			};
			make_jobs_available(jobs, 1);

			LateLast policy;
			policy.set_maxMem(workers);
			policy.assign_jobs(jobs, workers);

			Assert::IsTrue(workers[0].get_working_jobId() == "003");
			Assert::IsTrue(workers[1].get_working_jobId() == "002");
			Assert::IsTrue(workers[2].get_working_jobId() == "001");
		}

		TEST_METHOD(LateLast_PreemptsLateJobWhenOnTimeWorkIsAvailable)
		{
			std::vector<Job> jobs{
				create_job("001", 0, 5, 1, 1, 0),
				create_job("002", 0, 5, 1, 1, 10)
			};
			std::vector<Worker> workers{ create_worker("worker", 4) };
			make_jobs_available(jobs, 1);
			workers[0].set_job("001");

			LateLast policy;
			policy.set_maxMem(workers);
			policy.assign_jobs(jobs, workers);

			Assert::IsTrue(workers[0].get_working_jobId() == "002");
			Assert::IsTrue(workers[0].get_last_jobId() == "001");
		}

		TEST_METHOD(JobUpdateStates)
		{
			std::vector<Job> jobs{
				create_job("001", 5, 1, 1), //should be not_arrived
				create_job("002", 0, 3, 1),
				create_job("003", 0, 4, 4)
			};

			
			make_jobs_available(jobs, 0);
			
			jobs[1].work(0);

			make_jobs_available(jobs, 1);

			Assert::IsTrue(jobs[0].get_state() == JobStates::NOTARRIVED);
			Assert::IsTrue(jobs[1].get_state() == JobStates::RUNNING);
			Assert::IsTrue(jobs[2].get_state() == JobStates::WAITING);
		}

		TEST_METHOD(RecoveryResume_PreservesWorkCompletedBeforeFailure)
		{
			const Tick completionTick = run_recovery_scenario(RecoveryMode::RESUME);

			Assert::IsTrue(completionTick == 5);
		}

		TEST_METHOD(RecoveryRestart_RepeatsWorkCompletedBeforeFailure)
		{
			const Tick completionTick = run_recovery_scenario(RecoveryMode::RESTART);

			Assert::IsTrue(completionTick == 6);
		}
	};
}
