#include "MainWindowOpenGL.h"
#include "Log/Log.h"
#include "Rendering/RenderEngine.h"
#include "WindowUIOpenGL.h"

MainWindowOpenGL::MainWindowOpenGL(const std::string& name, glm::ivec2 resolution) :
	MainWindow(name, resolution) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        Log::Error("SDL2 initialization failed.");
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    m_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_resolution.x, m_resolution.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        Log::Error("SDL2 failed to create an OpenGL window.");
        exit(2);
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        Log::Error("SDL2 failed to initialize OpenGL context.");
        exit(3);
    }
    SDL_GL_MakeCurrent(m_window, m_glContext);
}

MainWindowOpenGL::~MainWindowOpenGL() {
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void MainWindowOpenGL::InitUI() {
    if (!m_ui) {
        m_ui = new WindowUIOpenGL(this, true);
    }
}

SDL_GLContext MainWindowOpenGL::GetOpenGLContext() const {
	return m_glContext;
}

void MainWindowOpenGL::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            int32_t newWidth = event.window.data1;
            int32_t newHeight = event.window.data2;
            RenderEngine::ResizeViewport({ newWidth, newHeight });
        }
    }
    MainWindow::HandleEvent(event);
}
