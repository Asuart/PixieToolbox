#pragma once
#include "Window/MainWindow.h"
#include "Rendering/RendererBase.h"
#include "Rendering/RenderAPI.h"
#include "Config.h"

class PixieToolboxCore {
public:
	static MainWindow* CreateMainWindow(const std::string& name, glm::ivec2 resolution, RenderAPI renderAPI);

private:
	static inline MainWindow* m_mainWindow = nullptr;
	static inline RendererBase* m_renderer = nullptr;
	static inline RenderAPI m_renderAPI = RenderAPI::Undefined;
};
