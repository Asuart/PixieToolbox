#pragma once
#include <PixieUIApplication/PixieUIApplication.h>
#include <PixieRendering/ResourceHandles.h>

namespace PixieToolbox {

class PixieToolboxApplication : public PixieUIApplication {
public:
	PixieToolboxApplication(const std::string& name, glm::ivec2 resolution, RenderAPI renderAPI, bool docking);

protected:
	TextureHandle m_texture;
	ComputeShaderHandle m_computeProgram;

	void OnStart() override;
	void OnClose() override;
	void OnBeforeDrawUI() override;
	void HandleEventAfterUI(const WindowEvent& sdlEvent) override;
};

}
