#include "WindowUI.h"

#include <vulkan/vulkan.hpp>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>

#include "Window/MainWindow.h"
#include "Window/MainWindowOpenGL.h"
#include "Window/MainWindowVulkan.h"
#include "Window/UIElement.h"
#include "Log/Log.h"

WindowUI::WindowUI(MainWindow* mainWindow, bool docking) :
	m_window(mainWindow), m_isDocking(docking) {
	IMGUI_CHECKVERSION();
}

WindowUI::~WindowUI() {
	for (size_t i = 0; i < m_elements.size(); i++) {
		delete m_elements[i];
	}
}

void WindowUI::HandleEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
	for (UIElement* element : m_elements) {
		element->HandleEvent(event);
	}
}

void WindowUI::AddElement(UIElement* window) {
	if (!window) {
		return;
	}
    m_elements.push_back(window);
}
