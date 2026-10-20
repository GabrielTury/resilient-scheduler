#include "Job.h"

void Job::update(Tick currentTick)
{
	bool bHasArrived = arrival >= currentTick;
	bool bCompleted = remainingTicks == 0;
	bool bLate = currentTick > deadline;

	if (bHasArrived && !bCompleted)
	{
		bActive = true;		
	}	


	state = bHasArrived ? JobStates::READY : JobStates::NOTARRIVED;
	state = bHasArrived && bCompleted ? JobStates::COMPLETED : state;
	state = bHasArrived && bLate && !bCompleted ? JobStates::LATE : JobStates::WAITING;
}

void Job::work(Tick tick)
{
	remainingTicks--;	

	completionTick = remainingTicks == 0 ? tick : -1;
}
