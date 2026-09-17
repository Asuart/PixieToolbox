#include "Timer.h"

#include <format>
#include <iostream>

namespace PixieToolbox {

Timer::Timer(const std::string& name) : m_name(name) {
	m_start = TimeMeasurement::Clock::now();
}

Timer::~Timer() {
	auto end = TimeMeasurement::Clock::now();
	double ms = std::chrono::duration<double, std::milli>(end - m_start).count();
	std::cout << std::format("{}: {:.3f} ms\n", m_name, ms);
}

} // namespace PixieToolbox
