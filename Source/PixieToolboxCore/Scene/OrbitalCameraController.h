#pragma once
#include "Camera.h"

class OrbitalCameraController {
public:
	OrbitalCameraController(Camera& camera, float moveSpeed = 1.0f);

	// Return true if camera moved.
	bool HandleUserInput();

protected:
	Camera& m_camera;
	float m_moveSpeed;
};
