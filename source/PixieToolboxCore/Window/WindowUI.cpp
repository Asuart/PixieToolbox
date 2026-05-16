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
#include "Utils/Log.h"

WindowUI::WindowUI(MainWindow* mainWindow, bool docking) :
	m_window(mainWindow), m_isDocking(docking) {
	IMGUI_CHECKVERSION();

	if (MainWindowOpenGL* window = dynamic_cast<MainWindowOpenGL*>(m_window)) {
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForOpenGL(window->GetSDLWindow(), window->GetOpenGLContext());
		ImGui_ImplOpenGL3_Init();
	}
	else if (MainWindowVulkan* window = dynamic_cast<MainWindowVulkan*>(m_window)) {
		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		VkDescriptorPool imguiPool;
		if (vkCreateDescriptorPool(window->GetDevice(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS) {
			Log::Error("Failed to create Vulkan descriptor pool.");
			exit(1);
		}

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForVulkan(window->GetSDLWindow());

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.ApiVersion = VK_API_VERSION_1_4;
		init_info.Instance = window->GetInstance();
		init_info.PhysicalDevice = window->GetPhysicalDevice();
		init_info.Device = window->GetDevice();
		init_info.Queue = window->GetQueue();
		init_info.RenderPass = window->GetRenderPass();
		init_info.DescriptorPool = imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Subpass = 0;

		ImGui_ImplVulkan_Init(&init_info);
	}
}

WindowUI::~WindowUI() {
	for (size_t i = 0; i < m_elements.size(); i++) {
		delete m_elements[i];
	}
	if (MainWindowOpenGL* window = dynamic_cast<MainWindowOpenGL*>(m_window)) {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
	}
	else if (MainWindowVulkan* window = dynamic_cast<MainWindowVulkan*>(m_window)) {
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
	}
	ImGui::DestroyContext();
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

void WindowUI::Draw() {
	if (MainWindowOpenGL* window = dynamic_cast<MainWindowOpenGL*>(m_window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (m_isDocking) {
			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
		}

		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
			windowFlags |= ImGuiWindowFlags_NoBackground;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &m_isDocking, windowFlags);

		ImGui::PopStyleVar(3);

		if (m_isDocking) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
		}

		for (size_t i = 0; i < m_elements.size(); i++) {
			m_elements[i]->Draw();
		}

		ImGui::End();

		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(window->GetSDLWindow(), window->GetOpenGLContext());
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	else if (MainWindowVulkan* window = dynamic_cast<MainWindowVulkan*>(m_window)) {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (m_isDocking) {
			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
		}

		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
			windowFlags |= ImGuiWindowFlags_NoBackground;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &m_isDocking, windowFlags);

		ImGui::PopStyleVar(3);

		if (m_isDocking) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
		}

		for (size_t i = 0; i < m_elements.size(); i++) {
			m_elements[i]->Draw();
		}

		ImGui::End();

		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), window->GetCommandBuffer());
	}
}