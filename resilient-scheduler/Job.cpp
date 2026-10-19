#include "Job.h"

void Job::update(Tick currentTick)
{
	if (arrival >= currentTick)
	{
		bActive = true;
	}

}

void Job::work()
{
	remainingTicks--;
}
