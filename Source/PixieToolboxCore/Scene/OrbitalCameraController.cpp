#include "OrbitalCameraController.h"

#include <PixieApplication/Time/ApplicationTime.h>
#include <PixieApplication/UserInput/UserInput.h>

#include "Camera.h"

OrbitalCameraController::OrbitalCameraController(Camera& camera, float moveSpeed) :
	m_camera(camera), m_moveSpeed(moveSpeed) {
}

bool OrbitalCameraController::HandleUserInput() {
	// const float scale = 10.0f * (UserInput::GetKey(SDLK_LSHIFT) ? 0.1f : 1.0f) * Time::deltaTime * m_moveSpeed;
	// const float rotationScale = (UserInput::GetKey(SDLK_LSHIFT) ? 0.1f : 1.0f) * Time::deltaTime;
	bool moved = false;
	// Transform& transform = m_camera.GetTransform();
	// if (UserInput::GetKey(SDLK_w)) {
	// 	transform.Translate(transform.GetForward() * scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDLK_s)) {
	// 	transform.Translate(transform.GetForward() * -scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDLK_d)) {
	// 	transform.Translate(transform.GetRight() * scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDLK_a)) {
	// 	transform.Translate(transform.GetRight() * -scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDLK_SPACE)) {
	// 	transform.Translate(transform.GetUp() * scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDLK_LCTRL)) {
	// 	transform.Translate(transform.GetUp() * -scale);
	// 	moved = true;
	// }
	// if (UserInput::GetMouseButton(SDL_BUTTON_RIGHT)) {
	// 	if (UserInput::mouseDeltaX) {
	// 		transform.RotateAroundAxis(glm::vec3(0, 1, 0), -(float)UserInput::mouseDeltaX * rotationScale);
	// 		transform.LookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForward(), glm::vec3(0, 1, 0));
	// 		moved = true;
	// 	}
	// 	if (UserInput::mouseDeltaY) {
	// 		transform.RotateAroundAxis(glm::vec3(1, 0, 0), -(float)UserInput::mouseDeltaY * rotationScale);
	// 		transform.LookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForward(), glm::vec3(0, 1, 0));
	// 		moved = true;
	// 	}
	// }
	return moved;
}
