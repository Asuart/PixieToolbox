#include "UI.h"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <vulkan/vulkan.hpp>

#include <PixieRenderer/Window/IWindow.h>

#include "UIOpenGL.h"
#include "UIVulkan.h"
#include "UIWindow.h"

namespace PixieToolbox {

std::unique_ptr<UI> UI::Create(IWindow* window, bool docking, RenderAPI api) {
	switch (api) {
	case RenderAPI::OpenGL:
		return std::make_unique<UIOpenGL>(window, docking);
	case RenderAPI::Vulkan:
		return std::make_unique<UIVulkan>(window, docking);
	default:
		return nullptr;
	}
}

UI::UI(PixieRenderer::IWindow* mainWindow, bool docking) : m_window(mainWindow), m_isDocking(docking) {
	IMGUI_CHECKVERSION();
}

UI::~UI() {
	for (size_t i = 0; i < m_windows.size(); i++) {
		delete m_windows[i];
	}
}

void UI::AddWindow(PixieToolbox::UIWindow* window) {
	if (!window) {
		return;
	}
	m_windows.push_back(window);
}

void UI::OnBeforeDrawFrame() {
	for (PixieToolbox::UIWindow* window : m_windows) {
		window->OnBeforeDraw();
	}
}

} // namespace PixieToolbox
