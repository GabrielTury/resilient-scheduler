#include "Job.h"

void Job::update(Tick currentTick)
{
	bool bHasArrived = arrival <= currentTick;
	bool bCompleted = remainingTicks == 0;
	bool bImpossible = state == JobStates::IMPOSSIBLE;

	bActive = (bHasArrived && !bCompleted && !bImpossible);

	if (bImpossible) return;
	
	bool bLate = currentTick > deadline;	
	
	if (bCompleted) { state = JobStates::COMPLETED; return; }
	if (bLate) { state = JobStates::LATE; return; }
	if (!bHasArrived) { state = JobStates::NOTARRIVED; return; }

	if (bTicking) {
		state = JobStates::RUNNING;
	} 
	else {
		state = JobStates::WAITING;
		waitingTicks++;
	}
}

void Job::work(Tick tick)
{
	remainingTicks--;	
	bTicking = true;
	state = JobStates::RUNNING;

	if (remainingTicks == 0)
	{
		bTicking = false;
		completionTick = tick;
		state = JobStates::COMPLETED;
	}
}

void Job::restart()
{
	remainingTicks = duration;
	this->pause();
}

void Job::finish()
{
	bFinished = true;
}

void Job::declare_impossible()
{
	state = JobStates::IMPOSSIBLE;
}

void Job::pause()
{
	bTicking = false;
}
