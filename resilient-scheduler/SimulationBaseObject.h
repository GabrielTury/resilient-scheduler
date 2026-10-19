#pragma once
#include "Types.h"

class SimulationBaseObject {
protected:
	bool bActive = false;
public:
	virtual void update(Tick tick) = 0;
	virtual const bool get_active() const { return bActive; };
};