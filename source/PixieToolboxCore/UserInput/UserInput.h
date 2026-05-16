#pragma once
#include "SDL2/SDL.h"

class UserInput {
public:
	static bool buttonPressed;
	static int32_t key;
	static int32_t scancode;
	static int32_t action;
	static int32_t mods;

	static bool mouseButtonPressed;
	static uint32_t mouseButton;
	static uint32_t mouseAction;
	static uint32_t mouseMods;

	static double mouseX;
	static double mouseY;
	static double mouseDeltaX;
	static double mouseDeltaY;

	static double mouseScrollX;
	static double mouseScrollY;

	static void Reset();

	static void SetInputWindow(SDL_Window* window);

	static int32_t GetKey(int32_t keyCode);
	static int32_t GetMouseButton(int32_t mouseButton);

protected:
	static SDL_Window* window;
};
