#pragma once
#include <cstdint>
using Tick = uint32_t;

enum ErrorCode {
	SUCCESS = 0,
	INVALID_COMMAND = -1,
	INVALID_OPTION = -2,
	EXECUTION_FAILED = -3
};

enum Policies {
	SJF = 0,
	FIFO = 1,
	RR = 2
};

enum class JobStates {
	NOTARRIVED = 0,
	READY = 1,
	COMPLETED = 2,
	LATE = 3,
	WAITING = 4,
};

