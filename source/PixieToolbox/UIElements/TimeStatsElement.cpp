#include "TimeStatsElement.h"

#include <imgui.h>

#include <Time/GlobalTimer.h>

TimeStatsElement::TimeStatsElement() {

}

void TimeStatsElement::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Stats", 0)) {
		for (const TimeMeasurement& timeMeasurement : GlobalTimer::GetTimers()) {
			double milli = (double)timeMeasurement.deltaTime.count() / 1000000.0f;
			ImGui::Text((timeMeasurement.name + std::string(": ") + std::to_string(milli) + "ms").c_str());
		}
	}
	ImGui::End();
}
