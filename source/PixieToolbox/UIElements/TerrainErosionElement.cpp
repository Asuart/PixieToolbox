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
#include <Config.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

TerrainErosionElement::TerrainErosionElement() :
	m_camera(glm::vec3(-128, 128, -128), glm::vec3(128, 0, 128), glm::vec3(0, 1, 0), glm::radians(39.6f), 16.0f / 9.0f, 1.0f, 5000.0f),
	m_cameraController(m_camera) {

	GlobalTimer::StartTimer("Sim Initialization");

	Texture<uint8_t> heightMapUInt8 = TextureLoader::LoadTextureUInt8Greyscale("example-height-map.png");

	int32_t desiredWidth = 1000;
	float mapAspect = Aspect(heightMapUInt8.GetResolution());
	int32_t desiredHeight = static_cast<int32_t>(desiredWidth / mapAspect);
	Texture<float> heightMap = TextureUtils::NormalizeTexture(heightMapUInt8, 0.0f, 1.0f);
	heightMap = TextureUtils::ResizeTexture(heightMap, { desiredWidth, desiredHeight });
	heightMap = TextureUtils::GaussianBlurFilter(heightMap, 64, 32.0f);
	heightMap = TextureUtils::NormalizeTexture(heightMap, 0.0f, 24.0f);

	Texture<float> noiseTexture = TextureUtils::SumTextures<float>({
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 0, { 8, 8 }, 1.0f),
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 1, { 16, 16 }, 0.5f),
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 2, { 32, 32 }, 0.25f),
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 3, { 64, 64 }, 0.125f),
		TextureGenerator::GeneratePerlinNoise({ desiredWidth, desiredHeight }, 4, { 128, 128 }, 0.0625f)
		});
	noiseTexture = TextureUtils::NormalizeTexture(noiseTexture);
	noiseTexture = TextureUtils::MultiplyTexture(noiseTexture, 3.0f);

	heightMap = TextureUtils::SumTextures<float>({ heightMap , noiseTexture });

	const float realSize = 5475.0f;
	const float scale = 0.1826484018f;
	m_sim.SetInitialHeightMap(heightMap.GetData(), desiredWidth, desiredHeight, scale * realSize, scale * realSize / mapAspect);

	m_frameBuffer = RenderEngine::CreateFrameBuffer({ 1280, 720 });
	m_terrainShader = ShaderLoader::LoadShader(
		"ErosionSimulationTerrainVertexShader.glsl",
		"ErosionSimulationTerrainFragmentShader.glsl"
	);
	m_waterShader = ShaderLoader::LoadShader(
		"ErosionSimulationWaterVertexShader.glsl",
		"ErosionSimulationWaterFragmentShader.glsl"
	);

	Mesh* grid = MeshGenerator::Grid({ 256.0f, 256.0f / mapAspect }, { desiredWidth, desiredHeight });
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
		RenderEngine::SetUniform1f(m_terrainShader, "uCellSizeX", m_sim.m_cellSizeX);
		RenderEngine::SetUniform1f(m_terrainShader, "uCellSizeY", m_sim.m_cellSizeY);

		RenderEngine::DrawMesh(m_gridMesh, m_terrainShader);

		RenderEngine::SetUniform3f(m_waterShader, "uCameraPos", m_camera.GetTransform().GetPosition());
		RenderEngine::SetUniformMat4f(m_waterShader, "mModel", glm::mat4(1.0f));
		RenderEngine::SetUniformMat4f(m_waterShader, "mView", m_camera.GetViewMatrix());
		RenderEngine::SetUniformMat4f(m_waterShader, "mProjection", m_camera.GetProjectionMatrix());

		RenderEngine::DrawMesh(m_gridMesh, m_waterShader);

		RenderEngine::UnbindFrameBuffer();

		ImVec2 p = ImGui::GetCursorScreenPos();

		ImGui::Image((void*)RenderEngine::GetInternalColorAttachmentID(m_frameBuffer), viewportResolution, {0.0, 1.0}, {1.0, 0.0});

		ImGui::SetCursorScreenPos(ImVec2(p.x + 20, p.y + 20));
		if (ImGui::Button("Save Texture")) {
			std::vector<uint8_t> bufferData = RenderEngine::GetShaderStorageBufferData(m_sim.m_buffers[static_cast<int32_t>(ErosionSimulatorGPU::BufferBinding::TerrainHeight)], 0, m_sim.m_resolutionX * m_sim.m_resolutionY * sizeof(float));
			Texture<float> heightMapFloat(reinterpret_cast<float*>(bufferData.data()), { m_sim.m_resolutionX, m_sim.m_resolutionY });
			
			float min = INFINITY;
			float max = -INFINITY;
			for (int32_t i = 0; i < m_sim.m_resolutionX * m_sim.m_resolutionY; i++) {
				min = glm::min(min, heightMapFloat.GetPixel(i));
				max = glm::max(max, heightMapFloat.GetPixel(i));
			}
			uint8_t* newData = new uint8_t[m_sim.m_resolutionX * m_sim.m_resolutionY];
			for (int32_t i = 0; i < m_sim.m_resolutionX * m_sim.m_resolutionY; i++) {
				newData[i] = static_cast<uint8_t>(255.0f * (heightMapFloat.GetPixel(i) - min) / (max - min));
			}

			stbi_write_png("D:/test.png", m_sim.m_resolutionX, m_sim.m_resolutionY, 1, newData, 0);

			delete[] newData;
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void TerrainErosionElement::HandleEvent(const SDL_Event& event) {

}
