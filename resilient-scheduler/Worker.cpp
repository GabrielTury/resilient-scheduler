#include "Worker.h"

void Worker::update(Tick tick)
{
}

void Worker::set_active(const bool active) {
	bActive = active;

	if (!active) this->free_worker();
}
