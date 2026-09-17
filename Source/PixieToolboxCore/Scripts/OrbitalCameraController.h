#pragma once
#include "Script.h"

#include <glm/glm.hpp>

namespace PixieToolbox {

class OrbitalCameraController : public Script {
  public:
	OrbitalCameraController(
	    glm::vec3 target = glm::vec3(0.0f),
	    float distance = 5.0f,
	    float sensitivity = 0.2f,
	    float zoomSpeed = 1.0f,
	    float panSpeed = 5.0f
	);

	void OnStart(Scene& scene, entt::entity entity) override;
	void OnUpdate(Scene& scene, entt::entity entity) override;

	glm::vec3 target{ 0.0f };
	float distance = 5.0f;
	float sensitivity = 0.2f;
	float zoomSpeed = 1.0f;
	float panSpeed = 5.0f;

	float minDistance = 0.1f;
	float maxDistance = 500.0f;

  private:
	float m_yaw = 0.0f;
	float m_pitch = 20.0f;
};

} // namespace PixieToolbox
