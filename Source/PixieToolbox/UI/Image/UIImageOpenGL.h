#pragma once
#include "UIImage.h"

#include <memory>

namespace PixieToolbox {

class UIImageOpenGL : public UIImage {
  public:
	explicit UIImageOpenGL(std::shared_ptr<IRenderer> renderer);
	virtual ~UIImageOpenGL();

	void SetTexture(TextureHandle texture) override;
	void SetFrameBuffer(FrameBufferHandle frameBuffer) override;
	ImTextureID GetTextureID() const override;

  private:
	std::shared_ptr<IRenderer> m_renderer;
	ImTextureID m_displayTexture = 0;
	TextureHandle m_currentTexture = {};
	FrameBufferHandle m_currentFrameBuffer = {};
};

} // namespace PixieToolbox
