#include "PixieToolbox.h"

#include <PixieUIApplication/Windows/DemoWindow.h>
#include <PixieUIApplication/Windows/TextureDisplayWindow.h>
#include <PixieUIApplication/Windows/ApplicationStatsWindow.h>

static const char* EXAMPLE_COMPUTE_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

void main() {
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(imgOutput);

    if (texelCoord.x < size.x && texelCoord.y < size.y) {
        vec2 uv = (vec2(texelCoord) + 0.5) / vec2(size);
        vec4 color = vec4(uv.x, uv.y, 0.0, 1.0);
        imageStore(imgOutput, texelCoord, color);
    }
}
)";

namespace PixieToolbox {

PixieToolboxApplication::PixieToolboxApplication(const std::string& name, glm::ivec2 resolution, RenderAPI renderAPI, bool docking) :
	PixieUIApplication(name, resolution, renderAPI, docking) {}

void PixieToolboxApplication::OnStart() {
	m_texture = m_renderer->CreateTexture(m_window->GetResolution(), TextureFormat::RGBA32f);
	m_computeProgram = m_renderer->CreateComputeShader(EXAMPLE_COMPUTE_SHADER_SOURCE);

	m_ui->AddWindow(new UI::TextureDisplayWindow(m_renderer, m_texture));
	m_ui->AddWindow(new UI::DemoWindow(m_renderer));
	m_ui->AddWindow(new UI::ApplicationStatsWindow(m_renderer));
}

void PixieToolboxApplication::OnClose() {

}

void PixieToolboxApplication::OnBeforeDrawUI() {
	glm::ivec2 textureResolution = m_renderer->GetTextureResolution(m_texture);
	int32_t workGroupsX = (textureResolution.x + 7) / 8;
	int32_t workGroupsY = (textureResolution.y + 7) / 8;

	m_renderer->BindTexture(m_computeProgram, "imgOutput", m_texture, 0);
	m_renderer->DispatchComputeShader(m_computeProgram, workGroupsX, workGroupsY, 1);
	m_renderer->MemoryBarriersAll();
}

void PixieToolboxApplication::HandleEventAfterUI(const WindowEvent& sdlEvent) {

}

}
