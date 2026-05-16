#pragma once
#include <Window/UIElement.h>
#include <Rendering/ResourceHandles.h>

class WindowUI;

class TextureDisplayElement : public UIElement {
public:
	TextureDisplayElement(TextureHandle texture);

	void Draw() override;

	void SetTexture(TextureHandle texture);

protected:
	FrameBufferHandle m_frameBuffer;
	TextureHandle m_targetTexture;
	ShaderHandle m_shader;
	MeshHandle m_screenPlane;
};
