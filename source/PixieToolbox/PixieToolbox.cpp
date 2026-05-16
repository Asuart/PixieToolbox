#include "PixieToolbox.h"

#include <Rendering/RenderEngine.h>
#include <Window/MainWindow.h>
#include <Window/WindowUI.h>
#include <Utils/ShaderLoader.h>

#include "UIElements/DemoElement.h"
#include "UIElements/TextureDisplayElement.h"
#include "UIElements/TerrainErosionElement.h"
#include "UIElements/TimeStatsElement.h"

PixieToolbox::PixieToolbox() : Application() {}

void PixieToolbox::OnStart() {
	m_texture = RenderEngine::CreateTexture(m_window->GetResolution(), TextureFormat::RGBA32f);
	m_computeProgram = ShaderLoader::LoadComputeShader("ExampleComputeShader.glsl");

	m_window->GetUI()->AddElement(new TextureDisplayElement(m_texture));
	m_window->GetUI()->AddElement(new TerrainErosionElement());
	m_window->GetUI()->AddElement(new DemoElement());
	m_window->GetUI()->AddElement(new TimeStatsElement());
}

void PixieToolbox::OnClose() {
}

void PixieToolbox::OnBeforeDrawFrame() {
	glm::ivec2 textureResolution = RenderEngine::GetTextureResolution(m_texture);
	int32_t workGroupsX = (textureResolution.x + 7) / 8;
	int32_t workGroupsY = (textureResolution.y + 7) / 8;

	RenderEngine::BindTexture(m_computeProgram, "imgOutput", m_texture, 0);
	RenderEngine::DispatchComputeShader(m_computeProgram, workGroupsX, workGroupsY, 1);
	RenderEngine::MemoryBarriersAll();
}

void PixieToolbox::OnAfterDrawFrame() {

}

void PixieToolbox::HandleEvent(SDL_Event& sdlEvent) {

}
