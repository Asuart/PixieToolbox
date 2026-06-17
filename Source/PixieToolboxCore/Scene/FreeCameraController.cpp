#include "FreeCameraController.h"

#include <PixieApplication/Time/ApplicationTime.h>
#include <PixieApplication/UserInput/UserInput.h>

FreeCameraController::FreeCameraController(Camera& camera, float moveSpeed) :
	m_camera(camera), m_moveSpeed(moveSpeed) {
}

bool FreeCameraController::HandleUserInput() {
	// const float scale = 100.0f * (UserInput::GetKey(SDL_SCANCODE_LSHIFT) ? 0.1f : 1.0f) * Time::deltaTime * m_moveSpeed;
	// const float rotationScale = (UserInput::GetKey(SDL_SCANCODE_LSHIFT) ? 0.05f : 0.2f) * Time::deltaTime;
	bool moved = false;
	
	// Transform& transform = m_camera.GetTransform();
	// if (UserInput::GetKey(SDL_SCANCODE_W)) {
	// 	transform.Translate(transform.GetForward() * scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDL_SCANCODE_S)) {
	// 	transform.Translate(transform.GetForward() * -scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDL_SCANCODE_D)) {
	// 	transform.Translate(transform.GetRight() * scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDL_SCANCODE_A)) {
	// 	transform.Translate(transform.GetRight() * -scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDL_SCANCODE_SPACE)) {
	// 	transform.Translate(transform.GetUp() * scale);
	// 	moved = true;
	// }
	// if (UserInput::GetKey(SDL_SCANCODE_LCTRL)) {
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
