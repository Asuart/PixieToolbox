#pragma once
#include <string>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "Application.h"

class WindowUI;

class MainWindow {
public:
    MainWindow(const std::string& name, glm::ivec2 resolution);
    virtual ~MainWindow();

    SDL_Window* GetSDLWindow() const;
    WindowUI* GetUI() const;
    glm::ivec2 GetResolution() const;
    bool GetShouldClose() const;

    void InitUI();
    void SetApplication(Application* application);
    void Close();

    virtual void Start();

protected:
    SDL_Window* m_window = nullptr;
    WindowUI* m_ui = nullptr;
    glm::ivec2 m_resolution = { 0, 0 };
	bool m_shouldClose = false;
    Application* m_application = nullptr;

	virtual void HandleEvent(const SDL_Event& event);
	virtual void SwapBuffers();

    void DrawFrame();
};
