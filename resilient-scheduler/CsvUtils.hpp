#pragma once
#include <string>

class CsvUtils {

template<typename T>
static T parse(const std::string& path);
};

template<typename T>
inline T CsvUtils::parse(const std::string& path)
{

	return T();
}
