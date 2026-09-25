#pragma once
#include "Script.h"

namespace PixieToolbox {

class FreeCameraController : public Script {
  public:
	FreeCameraController(float moveSpeed = 5.0f, float sensitivity = 0.1f);

	void OnStart(Scene& scene, entt::entity entity) override;
	void OnUpdate(Scene& scene, entt::entity entity) override;

	float moveSpeed = 1.0f;
	float sensitivity = 0.05f;
	float boostMultiplier = 4.0f;
	bool captureCursor = true;

  private:
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
};

} // namespace PixieToolbox
