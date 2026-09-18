#include "ImGuiBackendVulkan.h"

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>

namespace PixieToolbox {

void ImGuiBackendVulkan::Init(GLFWwindow* window, IRenderer* r) {
	m_renderer = dynamic_cast<RendererVulkan*>(r);
	if (!m_renderer)
		throw std::runtime_error("ImGuiBackendVulkan: not a Vulkan renderer");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForVulkan(window, true);

	VkDescriptorPoolSize poolSizes[] = {
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
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 },
	};
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
	poolInfo.poolSizeCount = IM_ARRAYSIZE(poolSizes);
	poolInfo.pPoolSizes = poolSizes;
	if (vkCreateDescriptorPool(m_renderer->GetDevice(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS)
		throw std::runtime_error("ImGui: descriptor pool failed");

	const uint32_t imgCount = m_renderer->GetSwapchainImageCount();

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.ApiVersion = VK_API_VERSION_1_0;
	init_info.Instance = m_renderer->GetInstance();
	init_info.PhysicalDevice = m_renderer->GetPhysicalDevice();
	init_info.Device = m_renderer->GetDevice();
	init_info.Queue = m_renderer->GetGraphicsQueue();
	init_info.DescriptorPool = m_pool;
	init_info.MinImageCount = imgCount;
	init_info.ImageCount = imgCount;
	init_info.PipelineInfoMain.RenderPass = m_renderer->GetPresentRenderPass();
	init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.PipelineInfoMain.Subpass = 0;

	ImGui_ImplVulkan_Init(&init_info);

	m_renderer->SetPresentOverlayHook([this]() {
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_renderer->GetCurrentFrameCommandBuffer());
	});
}

void ImGuiBackendVulkan::Shutdown() {
	if (m_renderer)
		m_renderer->SetPresentOverlayHook(nullptr);

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	if (m_pool != VK_NULL_HANDLE && m_renderer) {
		vkDestroyDescriptorPool(m_renderer->GetDevice(), m_pool, nullptr);
		m_pool = VK_NULL_HANDLE;
	}
}

void ImGuiBackendVulkan::NewFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiBackendVulkan::Render() {
	ImGui::Render();
}

} // namespace PixieToolbox
