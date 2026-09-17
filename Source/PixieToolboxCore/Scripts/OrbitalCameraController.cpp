#include "OrbitalCameraController.h"

#include <algorithm>
#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "PixieToolboxCore/Time/ApplicationTime.h"
#include "PixieToolboxCore/UserInput/UserInput.h"
#include "PixieToolboxCore/Scene/Components.h"
#include "PixieToolboxCore/Scene/Scene.h"

namespace PixieToolbox {

OrbitalCameraController::OrbitalCameraController(
    glm::vec3 target,
    float distance,
    float sensitivity,
    float zoomSpeed,
    float panSpeed
)
    : target(target), distance(distance), sensitivity(sensitivity), zoomSpeed(zoomSpeed), panSpeed(panSpeed) {
}

void OrbitalCameraController::OnStart(Scene& scene, entt::entity entity) {
	if (auto* tc = scene.TryGetComponent<TransformComponent>(entity)) {
		const glm::vec3 dir = tc->transform.GetPosition() - target;
		const float len = glm::length(dir);
		if (len > 1e-4f) {
			distance = len;
			m_yaw = glm::degrees(std::atan2(dir.z, dir.x));
			m_pitch = glm::degrees(std::asin(glm::clamp(dir.y / len, -1.0f, 1.0f)));
		}
	}
	UserInput::SetCursorCaptured(false);
}

void OrbitalCameraController::OnUpdate(Scene& scene, entt::entity entity) {
	auto* tc = scene.TryGetComponent<TransformComponent>(entity);
	if (!tc)
		return;

	Transform& t = tc->transform;
	const float dt = Time::deltaTime;

	const glm::dvec2 scroll = UserInput::GetMouseScroll();
	if (scroll.y != 0.0) {
		distance -= static_cast<float>(scroll.y) * zoomSpeed;
		distance = glm::clamp(distance, minDistance, maxDistance);
	}

	if (UserInput::IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
		const glm::dvec2 md = UserInput::GetMouseDelta();
		m_yaw -= static_cast<float>(md.x) * sensitivity;
		m_pitch -= static_cast<float>(md.y) * sensitivity;
		m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
	}

	const float pan = panSpeed * dt * (UserInput::IsKeyDown(GLFW_KEY_LEFT_SHIFT) ? 0.2f : 1.0f);

	if (UserInput::IsKeyDown(GLFW_KEY_W))
		target += t.GetForward() * pan;
	if (UserInput::IsKeyDown(GLFW_KEY_S))
		target += t.GetForward() * -pan;
	if (UserInput::IsKeyDown(GLFW_KEY_D))
		target += t.GetRight() * pan;
	if (UserInput::IsKeyDown(GLFW_KEY_A))
		target += t.GetRight() * -pan;
	if (UserInput::IsKeyDown(GLFW_KEY_SPACE))
		target += glm::vec3(0.0f, pan, 0.0f);
	if (UserInput::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
		target += glm::vec3(0.0f, -pan, 0.0f);

	// --- Позиция камеры из сферических координат ---
	const float yr = glm::radians(m_yaw);
	const float pr = glm::radians(m_pitch);
	const glm::vec3 offset{ distance * std::cos(pr) * std::cos(yr),
		                    distance * std::sin(pr),
		                    distance * std::cos(pr) * std::sin(yr) };
	const glm::vec3 camPos = target + offset;

	t.LookAt(camPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

} // namespace PixieToolbox
