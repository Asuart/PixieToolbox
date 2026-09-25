#include "FreeCameraController.h"

#include <algorithm>
#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "PixieToolboxCore/Scene/Components.h"
#include "PixieToolboxCore/Scene/Scene.h"
#include "PixieToolboxCore/Time/ApplicationTime.h"
#include "PixieToolboxCore/UserInput/UserInput.h"

namespace PixieToolbox {

FreeCameraController::FreeCameraController(float moveSpeed, float sensitivity)
    : moveSpeed(moveSpeed), sensitivity(sensitivity) {
}

void FreeCameraController::OnStart(Scene& scene, entt::entity entity) {
	Transform* t = nullptr;
	if (auto* tc = scene.TryGetComponent<TransformComponent>(entity)) {
		t = &tc->transform;
	}
	if (!t) {
		return;
	}

	const glm::vec3 f = glm::normalize(t->GetForward());
	m_yaw = glm::degrees(std::atan2(f.z, f.x));
	m_pitch = glm::degrees(std::asin(glm::clamp(f.y, -1.0f, 1.0f)));

	if (captureCursor) {
		UserInput::SetCursorCaptured(true);
	}
}

void FreeCameraController::OnUpdate(Scene& scene, entt::entity entity) {
	auto* tc = scene.TryGetComponent<TransformComponent>(entity);
	if (!tc) {
		return;
	}

	Transform& t = tc->transform;
	const float dt = Time::deltaTime;

	const float speed = moveSpeed * (UserInput::IsKeyDown(GLFW_KEY_LEFT_SHIFT) ? boostMultiplier : 1.0f) * dt;

	if (UserInput::IsKeyDown(GLFW_KEY_W)) {
		t.Translate(t.GetForward() * speed);
	}
	if (UserInput::IsKeyDown(GLFW_KEY_S)) {
		t.Translate(t.GetForward() * -speed);
	}
	if (UserInput::IsKeyDown(GLFW_KEY_D)) {
		t.Translate(t.GetRight() * speed);
	}
	if (UserInput::IsKeyDown(GLFW_KEY_A)) {
		t.Translate(t.GetRight() * -speed);
	} 
	if (UserInput::IsKeyDown(GLFW_KEY_SPACE)) {
		t.Translate(t.GetUp() * speed);
	}
	if (UserInput::IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
		t.Translate(t.GetUp() * -speed);
	}

	if (UserInput::IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
		const glm::dvec2 md = UserInput::GetMouseDelta();

		m_yaw += static_cast<float>(md.x) * sensitivity;
		m_pitch -= static_cast<float>(md.y) * sensitivity;
		m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

		const float yr = glm::radians(m_yaw);
		const float pr = glm::radians(m_pitch);
		const glm::vec3 forward{ std::cos(pr) * std::cos(yr), std::sin(pr), std::cos(pr) * std::sin(yr) };
		const glm::vec3 pos = t.GetPosition();
		t.LookAt(pos, pos + forward, glm::vec3(0.0f, 1.0f, 0.0f));
	}
}

} // namespace PixieToolbox
