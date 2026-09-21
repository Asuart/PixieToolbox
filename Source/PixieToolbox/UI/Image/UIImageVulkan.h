#pragma once
#include "UIImage.h"

#include <memory>

#include <vulkan/vulkan.h>

#include <PixieRenderer/Renderer/IRenderer.h>

namespace PixieToolbox {

class UIImageVulkan : public UIImage {
  public:
	explicit UIImageVulkan(std::shared_ptr<IRenderer> renderer);
	virtual ~UIImageVulkan();

	void SetTexture(TextureHandle texture) override;
	void SetFrameBuffer(FrameBufferHandle frameBuffer) override;
	ImTextureID GetTextureID() const override;

  private:
	std::shared_ptr<IRenderer> m_renderer;
	VkDescriptorSet m_displayTexture = VK_NULL_HANDLE;
	TextureHandle m_currentTexture = {};
	FrameBufferHandle m_currentFrameBuffer = {};
};

} // namespace PixieToolbox
