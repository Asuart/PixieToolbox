#pragma once
#include <SDL2/SDL.h>

class UIElement {
public:
	virtual void Draw() = 0;
	virtual void HandleEvent(const SDL_Event& event) {};

protected:
	UIElement() {};
};
