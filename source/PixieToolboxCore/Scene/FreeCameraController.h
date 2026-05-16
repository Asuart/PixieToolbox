#pragma once
#include "Camera.h"
#include <SDL2/SDL.h>

class FreeCameraController {
public:
	FreeCameraController(Camera& camera, float moveSpeed = 1.0f);

	// Return true if camera moved.
	bool HandleUserInput();

protected:
	Camera& m_camera;
	float m_moveSpeed;
};
