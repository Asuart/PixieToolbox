#pragma once
#include "TimeMeasurement.h"

namespace PixieToolbox {

class Timer {
  public:
	Timer(const std::string& name);
	~Timer();

  private:
	std::string m_name;
	TimeMeasurement::Clock::time_point m_start;
};

} // namespace PixieToolbox
