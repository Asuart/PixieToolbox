#include "UserInput.h"

SDL_Window* UserInput::window = nullptr;

bool UserInput::buttonPressed = false;
int32_t UserInput::key = 0;
int32_t UserInput::scancode = 0;
int32_t UserInput::action = 0;
int32_t UserInput::mods = 0;

bool UserInput::mouseButtonPressed;
uint32_t UserInput::mouseButton;
uint32_t UserInput::mouseAction;
uint32_t UserInput::mouseMods;

double UserInput::mouseX = 0;
double UserInput::mouseY = 0;
double UserInput::mouseDeltaX = 0;
double UserInput::mouseDeltaY = 0;

double UserInput::mouseScrollX = 0;
double UserInput::mouseScrollY = 0;

void UserInput::Reset() {
	buttonPressed = false;
	mouseButtonPressed = false;
	mouseDeltaX = 0;
	mouseDeltaY = 0;
	mouseScrollX = 0;
	mouseScrollY = 0;
}

int32_t UserInput::GetKey(int32_t keyCode) {
	const Uint8* state = SDL_GetKeyboardState(NULL);
	return state[keyCode];
}

int32_t UserInput::GetMouseButton(int32_t mouseButton) {
	return 0;
}

void UserInput::SetInputWindow(SDL_Window* _window) {
	window = _window;
}
