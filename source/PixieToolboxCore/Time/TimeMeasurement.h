#pragma once
#include <string>
#include <chrono>

struct TimeMeasurement {
	std::string name;
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;
	std::chrono::nanoseconds deltaTime;
};
