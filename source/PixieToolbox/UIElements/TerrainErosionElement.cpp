#include "TerrainErosionElement.h"

#include <imgui.h>

#include <Rendering/RenderEngine.h>
#include <Mesh/MeshGenerator.h>
#include <Utils/ShaderLoader.h>
#include <Log/Log.h>
#include <Time/GlobalTimer.h>
#include <Texture/TextureLoader.h>
#include <Texture/TextureUtils.h>
#include <Texture/TextureGenerator.h>

TerrainErosionElement::TerrainErosionElement() :
	m_camera(glm::vec3(-128, 128, -128), glm::vec3(128, 0, 128), glm::vec3(0, 1, 0), glm::radians(39.6f), 16.0f / 9.0f, 1.0f, 5000.0f),
	m_cameraController(m_camera) {

	GlobalTimer::StartTimer("Sim Initialization");

	Texture<float> heightMap = TextureLoader::LoadTextureFloatGreyscale("example-height-map.png");

	int32_t desiredWidth = 1024;
	float mapAspect = Aspect(heightMap.GetResolution());
	int32_t desiredHeight = static_cast<int32_t>(desiredWidth / mapAspect);
	heightMap = TextureUtils::ResizeTexture(heightMap, { desiredWidth, desiredHeight });
	heightMap = TextureUtils::GaussianBlurFilter(heightMap, 16, 16.0f);
	heightMap = TextureUtils::MultiplyTexture(heightMap, 10.0f);

	Texture<float> noiseTexture = TextureUtils::SumTextures<float>({
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 0, { 8, 8 }, 1.0f),
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 1, { 16, 16 }, 0.5f),
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 2, { 32, 32 }, 0.25f),
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 3, { 64, 64 }, 0.125f),
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 4, { 128, 128 }, 0.0625f)
		});
	noiseTexture = TextureUtils::NormalizeTexture(noiseTexture);
	noiseTexture = TextureUtils::MultiplyTexture(noiseTexture, 5.0f);

	heightMap = TextureUtils::SumTextures<float>({ heightMap , noiseTexture });

	m_sim.SetInitialHeightMap(heightMap.GetData(), desiredWidth, desiredHeight, 100.0f, 100.0f / mapAspect);

	m_frameBuffer = RenderEngine::CreateFrameBuffer({ 1280, 720 });
	m_terrainShader = ShaderLoader::LoadShader(
		"ErosionSimulationTerrainVertexShader.glsl",
		"ErosionSimulationTerrainFragmentShader.glsl"
	);
	m_waterShader = ShaderLoader::LoadShader(
		"ErosionSimulationWaterVertexShader.glsl",
		"ErosionSimulationWaterFragmentShader.glsl"
	);

	Mesh* grid = MeshGenerator::Grid({ 256, 256 * (6800.0f / 10000.0f) }, { desiredWidth, desiredHeight });
	m_gridMesh = RenderEngine::LoadMesh(grid);
	delete grid;

	GlobalTimer::StopTimer("Sim Initialization");
}

void TerrainErosionElement::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("Viewport##")).c_str())) {
		if (ImGui::IsWindowFocused()) {
			m_cameraController.HandleUserInput();
		}

		m_sim.Step();

		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };

		m_camera.SetAspect(Aspect(glmViewportResolution));

		RenderEngine::ResizeFrameBuffer(m_frameBuffer, glmViewportResolution);
		RenderEngine::BindFrameBuffer(m_frameBuffer);
		RenderEngine::ClearFrameBuffer();

		RenderEngine::SetUniform3f(m_terrainShader, "uCameraPos", m_camera.GetTransform().GetPosition());
		RenderEngine::SetUniformMat4f(m_terrainShader, "mModel", glm::mat4(1.0f));
		RenderEngine::SetUniformMat4f(m_terrainShader, "mView", m_camera.GetViewMatrix());
		RenderEngine::SetUniformMat4f(m_terrainShader, "mProjection", m_camera.GetProjectionMatrix());
		RenderEngine::SetUniform1f(m_terrainShader, "uDeltaTime", m_sim.m_dt);

		RenderEngine::DrawMesh(m_gridMesh, m_terrainShader);

		RenderEngine::SetUniform3f(m_waterShader, "uCameraPos", m_camera.GetTransform().GetPosition());
		RenderEngine::SetUniformMat4f(m_waterShader, "mModel", glm::mat4(1.0f));
		RenderEngine::SetUniformMat4f(m_waterShader, "mView", m_camera.GetViewMatrix());
		RenderEngine::SetUniformMat4f(m_waterShader, "mProjection", m_camera.GetProjectionMatrix());

		RenderEngine::DrawMesh(m_gridMesh, m_waterShader);

		RenderEngine::UnbindFrameBuffer();

		ImGui::Image((void*)RenderEngine::GetInternalColorAttachmentID(m_frameBuffer), viewportResolution, {0.0, 1.0}, {1.0, 0.0});
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void TerrainErosionElement::HandleEvent(const SDL_Event& event) {

}
