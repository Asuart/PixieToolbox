#include "PixieToolboxCore.h"

#include <iostream>

#include "Window/MainWindowOpenGL.h"
#include "Window/MainWindowVulkan.h"
#include "Rendering/RenderEngine.h"

MainWindow* PixieToolboxCore::CreateMainWindow(const std::string& name, glm::ivec2 resolution, RenderAPI renderAPI) {
	if (PixieToolboxCore::m_mainWindow) {
		std::cout << "Main window is already created. render api: " << to_string(renderAPI) << "\n";
		return nullptr;
	}
	MainWindow* mainWindow;
	switch (renderAPI) {
	case RenderAPI::OpenGL:
		mainWindow = new MainWindowOpenGL(name, resolution);
		break;
	case RenderAPI::Vulkan:
		mainWindow = new MainWindowVulkan(name, resolution);
		break;
	default:
		std::cout << "Failed to create window for api: " << to_string(renderAPI) << "\n";
		return nullptr;
	}
	RenderEngine::Initialize(mainWindow, renderAPI);
	mainWindow->InitUI();
	return mainWindow;
}
