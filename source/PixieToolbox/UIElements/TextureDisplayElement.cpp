#include "TextureDisplayElement.h"

#include <imgui.h>
#include <string>
#include <memory>

#include "Math/MathBase.h"
#include "Rendering/RenderEngine.h"
#include "Window/WindowUI.h"
#include "Mesh/MeshGenerator.h"
#include "Utils/ShaderLoader.h"

TextureDisplayElement::TextureDisplayElement(TextureHandle texture) :
	m_targetTexture(texture) {
	m_frameBuffer = RenderEngine::CreateFrameBuffer({ 1280, 720 });
	m_shader = ShaderLoader::LoadShader(
		"TextureViewerQuadVertexShader.glsl",
		"TextureViewerQuadFragmentShader.glsl"
	);

	Mesh* mesh = MeshGenerator::Quad({ 0.0f, 0.0f }, { 1.0f, 1.0f });
	m_screenPlane = RenderEngine::LoadMesh(mesh);
	delete mesh;
}

void TextureDisplayElement::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("Texture View##")).c_str())) {
		if (ImGui::IsWindowFocused()) {}

		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };

		glm::vec2 pos(0.0f, 0.0f), size(1.0f, 1.0f);
		float textureAspect = Aspect(RenderEngine::GetTextureResolution(m_targetTexture));
		float viewportAspect = Aspect(glmViewportResolution);
		if (viewportAspect > textureAspect) {
			size.x = textureAspect / viewportAspect;
			pos.x = (1.0f - size.x) * 0.5f;
		}
		else {
			size.y = viewportAspect / textureAspect;
			pos.y = (1.0f - size.y) * 0.5f;
		}

		RenderEngine::ResizeFrameBuffer(m_frameBuffer, glmViewportResolution);
		RenderEngine::BindFrameBuffer(m_frameBuffer);
		RenderEngine::ClearFrameBuffer();

		RenderEngine::SetUniform2f(m_shader, "uPos", pos);
		RenderEngine::SetUniform2f(m_shader, "uSize", size);
		RenderEngine::BindTexture(m_shader, "displayTexture", m_targetTexture, 0);
		RenderEngine::DrawMesh(m_screenPlane, m_shader);

		RenderEngine::UnbindFrameBuffer();

		ImGui::Image((void*)RenderEngine::GetInternalColorAttachmentID(m_frameBuffer), viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void TextureDisplayElement::SetTexture(TextureHandle texture) {
	m_targetTexture = texture;
}
