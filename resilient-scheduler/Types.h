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



