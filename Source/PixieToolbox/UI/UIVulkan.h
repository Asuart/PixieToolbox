#pragma once
#include "UI.h"

#include <vulkan/vulkan.h>

namespace PixieRenderer {
class WindowVulkan;
}

namespace PixieToolbox {

class UIVulkan : public UI {
  public:
	UIVulkan(PixieRenderer::WindowVulkan* window, bool docking);
	~UIVulkan();

	void Draw();
	UIImage* CreateUIImage(
	    PixieRenderer::IRenderer* renderer,
	    PixieRenderer::FrameBufferHandle handle
	) override;
	UIImage* CreateUIImage(PixieRenderer::IRenderer* renderer, PixieRenderer::TextureHandle handle)
	    override;

		private:
			VkDescriptorPool m_pool = VK_NULL_HANDLE;
};

} // namespace PixieToolbox
