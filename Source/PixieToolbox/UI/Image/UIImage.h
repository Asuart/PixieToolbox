#pragma once
#include <imgui.h>

#include <PixieRenderer/ResourceManager/ResourceHandles.h>

namespace PixieRenderer {
class IRenderer;
}

namespace PixieToolbox {

using namespace PixieRenderer;

class UIImage {
  public:
	virtual ~UIImage() = default;

	virtual void SetTexture(TextureHandle texture) = 0;
	virtual void SetFrameBuffer(FrameBufferHandle frameBuffer) = 0;
	virtual ImTextureID GetTextureID() const = 0;
};

} // namespace PixieToolbox
