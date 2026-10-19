#pragma once
#include <string>
class Logger
{
private:
	std::string basePath;
	std::string summaryFile = basePath + "summary.json";
	std::string traceFile = basePath + "trace.csv";

public:
	Logger(std::string bPath, std::string sumFile, std::string tFile) : basePath(bPath), summaryFile(sumFile), traceFile(tFile) {};

	void write_summary();
};

