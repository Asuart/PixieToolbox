#include "TerrainErosionElement.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <imgui.h>

#include <Rendering/RenderEngine.h>
#include <Mesh/MeshGenerator.h>
#include <Utils/ShaderLoader.h>
#include <Utils/Log.h>
#include <Time/GlobalTimer.h>

std::vector<float> uploadHeightMap(const std::string& path) {
	int width, height, channels;
	uint8_t* originalImage = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);
	if (!originalImage) {
		std::cout << "stb failed to load an image: " << path << "\n";
		exit(0);
	}
	std::vector<float> heightMap(width * height);
	for (int32_t i = 0; i < heightMap.size(); i++) {
		heightMap[i] = originalImage[i * channels];
	}

	return heightMap;
}

std::vector<float> uploadHeightMapWithDesiredResolution(const std::string& path, int32_t desiredWidth, int32_t desiredHeight) {
	int width, height, channels;
	uint8_t* originalImage = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);
	if (!originalImage) {
		std::cout << "stb failed to load an image: " << path << "\n";
		exit(0);
	}

	float max = -INFINITY;
	float min = INFINITY;

	std::vector<float> heightMap(desiredWidth * desiredHeight);
	float rescaleX = static_cast<float>(desiredWidth) / static_cast<float>(width);
	float rescaleY = static_cast<float>(desiredHeight) / static_cast<float>(height);
	for (int32_t y = 0, pixel = 0; y < desiredHeight; y++) {
		for (int32_t x = 0; x < desiredWidth; x++, pixel++) {
			int32_t x0 = std::min(static_cast<int32_t>(x / rescaleX), width - 1);
			int32_t y0 = std::min(static_cast<int32_t>(y / rescaleY), height - 1);
			int32_t x1 = (x0 + 1) >= width ? x0 : (x0 + 1);
			int32_t y1 = (y0 + 1) >= height ? y0 : (y0 + 1);

			float u = (x / rescaleX) - static_cast<int32_t>(x / rescaleX);
			float v = (y / rescaleY) - static_cast<int32_t>(y / rescaleY);

			assert(u >= 0.0f && u <= 1.0f);
			assert(v >= 0.0f && v <= 1.0f);

			float v00 = std::max(0.0f, std::log10((float)originalImage[(y0 * width + x0) * channels])) * 15.0f + 2.0f;
			float v10 = std::max(0.0f, std::log10((float)originalImage[(y0 * width + x1) * channels])) * 15.0f + 2.0f;
			float v01 = std::max(0.0f, std::log10((float)originalImage[(y1 * width + x0) * channels])) * 15.0f + 2.0f;
			float v11 = std::max(0.0f, std::log10((float)originalImage[(y1 * width + x1) * channels])) * 15.0f + 2.0f;

			float bilinearlyInterpolated =
				v00 * (1.0f - u) * (1.0f - v) +
				v10 * u * (1.0f - v) +
				v01 * (1.0f - u) * v +
				v11 * u * v;

			heightMap[pixel] = bilinearlyInterpolated;

			max = fmaxf(max, bilinearlyInterpolated);
			min = fminf(min, bilinearlyInterpolated);
		}
	}

	std::cout << "range: " << min << "/" << max << "\n";

	stbi_image_free(originalImage);

	int32_t range = 16;

	std::vector<float> blurred(heightMap.size());
	for (int32_t y = 0, pixel = 0; y < desiredHeight; y++) {
		for (int32_t x = 0; x < desiredWidth; x++, pixel++) {
			float sum = 0.0f;
			float weight = 0.0f;
			for (int32_t r = -range; r <= range; r++) {
				if (x + r >= 0 && x + r < desiredWidth) {
					sum += heightMap[pixel + r];
					weight += 1.0f;
				}
			}
			blurred[pixel] = sum / weight;
		}
	}

	std::swap(heightMap, blurred);

	for (int32_t y = 0, pixel = 0; y < desiredHeight; y++) {
		for (int32_t x = 0; x < desiredWidth; x++, pixel++) {
			float sum = 0.0f;
			float weight = 0.0f;
			for (int32_t r = -range; r <= range; r++) {
				if (y + r >= 0 && y + r < desiredHeight) {
					sum += heightMap[pixel + r * desiredWidth];
					weight += 1.0f;
				}
			}
			blurred[pixel] = sum / weight;
		}
	}

	std::swap(heightMap, blurred);

	return heightMap;
}


TerrainErosionElement::TerrainErosionElement() :
	m_camera(glm::vec3(-128, 128, -128), glm::vec3(128, 0, 128), glm::vec3(0, 1, 0), glm::radians(39.6f), 16.0f / 9.0f, 0.01f, 1000.0f),
	m_cameraController(m_camera), m_sim(1, 1, 6700.0f / 10, 4590.0f / 10) {

	GlobalTimer::StartTimer("Sim Initialization");

	int32_t desiredWidth = 6700;
	int32_t desiredHeight = static_cast<int32_t>(desiredWidth * (4590.0f / 6700.0f));
	std::vector<float> image = uploadHeightMapWithDesiredResolution("C:/Users/asuart/Desktop/koblenz.map.blurred.cut.2.bmp", desiredWidth, desiredHeight);
	//std::vector<float> image = uploadHeightMap("C:/Users/asuart/Desktop/koblenz.map.blurred.bmp");
	m_sim.SetInitialHeightMap(image.data(), desiredWidth, desiredHeight);

	//auto heightData = RenderEngine::GetShaderStorageBufferData(m_sim.m_terrainHeight, 0, m_sim.m_cellsCount * sizeof(float));
	//m_terrainHeight = RenderEngine::LoadTexture((float*)heightData.data(), { m_sim.m_resolutionX, m_sim.m_resolutionY });

	m_frameBuffer = RenderEngine::CreateFrameBuffer({ 1280, 720 });
	m_terrainShader = ShaderLoader::LoadShader(
		"ErosionSimulationTerrainVertexShader.glsl",
		"ErosionSimulationTerrainFragmentShader.glsl"
	);
	Mesh* grid = MeshGenerator::Grid({ 256, 256 * (6341.0f / 10000.0f) }, { desiredWidth, desiredHeight });
	m_terrainMesh = RenderEngine::LoadMesh(grid);
	delete grid;

	GlobalTimer::StopTimer("Sim Initialization");
}

void TerrainErosionElement::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("Viewport##")).c_str())) {
		if (ImGui::IsWindowFocused()) {
			m_cameraController.HandleUserInput();
		}

		GlobalTimer::StartTimer("Sim Step");
		m_sim.Step();
		GlobalTimer::StopTimer("Sim Step");

		//auto heightData = RenderEngine::GetShaderStorageBufferData(m_sim.m_terrainHeight, 0, m_sim.m_cellsCount * sizeof(float));
		//RenderEngine::LoadTexture(m_terrainHeight, (float*)heightData.data(), { m_sim.m_resolutionX, m_sim.m_resolutionY });

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
		RenderEngine::BindTexture(m_terrainShader, "heightMap", m_terrainHeight, 0);

		RenderEngine::DrawMesh(m_terrainMesh, m_terrainShader);

		RenderEngine::UnbindFrameBuffer();

		ImGui::Image((void*)RenderEngine::GetInternalColorAttachmentID(m_frameBuffer), viewportResolution, {0.0, 1.0}, {1.0, 0.0});
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void TerrainErosionElement::HandleEvent(const SDL_Event& event) {

}
