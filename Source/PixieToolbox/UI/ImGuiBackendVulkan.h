#pragma once
#include "ImGuiBackend.h"

#include <PixieRenderer/Renderer/Vulkan/RendererVulkan.h>

namespace PixieToolbox {

class ImGuiBackendVulkan : public ImGuiBackend {
  public:
	void Init(GLFWwindow* window, IRenderer* r) override;
	void Shutdown() override;
	void NewFrame() override;
	void Render() override;

  private:
	RendererVulkan* m_renderer = nullptr;
	VkDescriptorPool m_pool = VK_NULL_HANDLE;
};

} // namespace PixieToolbox
