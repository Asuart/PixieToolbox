#include "MainWindow.h"

#include "WindowUI.h"
#include "Utils/Log.h"
#include "Time/EngineTime.h"
#include "Time/GlobalTimer.h"
#include "UserInput/UserInput.h"

MainWindow::MainWindow(const std::string& name, glm::ivec2 resolution)
    : m_resolution(resolution) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        Log::Error("SDL2 initialization failed.");
        exit(1);
    }
}

MainWindow::~MainWindow() {
    if (m_application) {
        delete m_application;
    }
    if (m_ui) {
        delete m_ui;
    }
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

SDL_Window* MainWindow::GetSDLWindow() const {
    return m_window;
}

WindowUI* MainWindow::GetUI() const {
    return m_ui;
}

glm::ivec2 MainWindow::GetResolution() const {
    return m_resolution;
}

bool MainWindow::GetShouldClose() const {
    return m_shouldClose;
}

void MainWindow::InitUI() {
    if (!m_ui) {
        m_ui = new WindowUI(this, true);
    }
}

void MainWindow::SetApplication(Application* application) {
    m_application = application;
    application->m_window = this;
}

void MainWindow::Close() {
    m_shouldClose = true;
}

void MainWindow::Start() {
    if (!m_application) {
        Log::Error("Main window failed to start. Application is not set.");
        return;
    }
    m_application->OnStart();
    while(!m_shouldClose) {
        GlobalTimer::StartTimer("Frame Time");

        Time::Update();
        m_application->OnBeforeDrawFrame();
        DrawFrame();
        SwapBuffers();
        m_application->OnAfterDrawFrame();

        UserInput::Reset();
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            HandleEvent(sdlEvent);
        }

        GlobalTimer::StopTimer("Frame Time");
    }
    m_application->OnClose();
}

void MainWindow::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        Close();
    }
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_window)) {
    	Close();
    }

    UserInput::HandleEvent(event);

    m_ui->HandleEvent(event);
    m_application->HandleEvent(event);
}

void MainWindow::SwapBuffers() {
    SDL_GL_SwapWindow(m_window);
}

void MainWindow::DrawFrame() {
    m_ui->Draw();
}
