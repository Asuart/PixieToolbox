#pragma once
#include <Application.h>
#include <Rendering/ResourceHandles.h>

class PixieToolbox : public Application {
public:
	PixieToolbox();

protected:
	TextureHandle m_texture;
	ComputeShaderHandle m_computeProgram;

	void OnStart() override;
	void OnClose() override;
	void OnBeforeDrawFrame() override;
	void OnAfterDrawFrame() override;
	void HandleEvent(const SDL_Event& sdlEvent) override;
};
