#include "Failure.h"

void Failure::update(Tick tick)
{
	if (tick >= start && tick <= (start + duration)) bActive = true;
}
