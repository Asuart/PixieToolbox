#pragma once
#include "UI.h"

#include <vulkan/vulkan.h>

namespace PixieToolbox {

class UIVulkan : public UI {
  public:
	UIVulkan(IWindow* window, bool docking);
	~UIVulkan();

	void Draw();
	UIImage* CreateUIImage(std::shared_ptr<IRenderer> renderer, FrameBufferHandle handle) override;
	UIImage* CreateUIImage(std::shared_ptr<IRenderer> renderer, TextureHandle handle) override;

  private:
	VkDescriptorPool m_pool = VK_NULL_HANDLE;
};

} // namespace PixieToolbox
