#pragma once
#include <cstdint>
using Tick = uint32_t;
using Quantum = uint16_t;

enum ErrorCode {
	SUCCESS = 0,
	INVALID_COMMAND = -1,
	INVALID_OPTION = -2,
	EXECUTION_FAILED = -3,
	INVALID_FILE = -4
};

enum class Policies {
	SJF = 0,
	FIFO = 1,
	RR = 2,
	LateLast = 3
};

enum class JobStates {
	NOTARRIVED = 0,
	WAITING = 1,
	RUNNING = 2,
	LATE = 3,
	COMPLETED = 4,
	IMPOSSIBLE = 5
};

enum class RecoveryMode {
	RESUME = 0,
	RESTART = 1
};