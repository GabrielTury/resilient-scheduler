#include "Failure.h"

void Failure::update(Tick tick)
{
	bActive = tick >= start && tick - start < duration;
}
