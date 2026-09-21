#pragma once
#include "PixieToolbox/UI/UIWindow.h"

#include <PixieRenderer/Renderer/IRenderer.h>
#include <PixieRenderer/ResourceManager/ResourceHandles.h>

#include "PixieToolbox/UI/Image/UIImage.h"

namespace PixieToolbox {

class TextureDisplayWindow : public UIWindow {
  public:
	TextureDisplayWindow(UI* ui, std::shared_ptr<IRenderer> renderer, TextureHandle texture);
	TextureDisplayWindow(UI* ui, std::shared_ptr<IRenderer> renderer, FrameBufferHandle frameBuffer);
	virtual ~TextureDisplayWindow();

	virtual void OnBeforeDraw() override;
	void Draw() override;

	void SetTexture(TextureHandle texture);
	void SetFrameBuffer(FrameBufferHandle frameBuffer);

  protected:
	FrameBufferHandle m_frameBuffer;
	TextureHandle m_targetTexture;
	FrameBufferHandle m_targetFrameBuffer;
	MaterialHandle m_shader;
	MeshHandle m_screenPlane;
	BufferHandle m_planeUBO;
	glm::uvec2 m_viewportResolution{};
	glm::uvec2 m_appliedResolution{};
	UIImage* m_image = nullptr;
	bool m_resolutionChanged = true;

	float Aspect(glm::ivec2 resolution);

	void CreateShader();
	void CreateScreenPlane();
	void UpdatePlaneUBO();
};

} // namespace PixieToolbox
