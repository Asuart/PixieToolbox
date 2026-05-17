#pragma once
#include <SDL2/SDL.h>

class MainWindow;

class Application {
protected:
	MainWindow* m_window;

	virtual void OnStart() {}
	virtual void OnClose() {}
	virtual void OnBeforeDrawFrame() {}
	virtual void OnAfterDrawFrame() {}
	virtual void HandleEvent(const SDL_Event& sdlEvent) {}

	friend class MainWindow;
	friend class MainWindowOpenGL;
	friend class MainWindowVulkan;
};
