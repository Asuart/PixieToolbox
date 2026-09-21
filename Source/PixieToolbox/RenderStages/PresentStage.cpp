#include "PresentStage.h"

#include <PixieRenderer/Material/PresentMaterial.h>
#include <PixieRenderer/RenderGraph/RenderGraphBuilder.h>
#include <PixieRenderer/RenderGraph/RenderGraphContext.h>
#include <PixieRenderer/Renderer/IRenderer.h>

namespace PixieToolbox {

PresentStage::PresentStage(std::shared_ptr<IRenderer> renderer, RGResource input)
    : IRenderStage(renderer), m_input(input) {
	PresentMaterial presentMat;
	MaterialHandle presentMatHandle = renderer->CreateMaterial(&presentMat);

	Mesh fsTri;
	fsTri.vertexes.resize(3);
	fsTri.vertexes[0].position = { -1.0f, -1.0f, 0.0f };
	fsTri.vertexes[1].position = { 3.0f, -1.0f, 0.0f };
	fsTri.vertexes[2].position = { -1.0f, 3.0f, 0.0f };
	fsTri.indexes = { 0, 1, 2 };
	MeshHandle fsTriHandle = renderer->CreateMesh(&fsTri);
}

std::string_view PresentStage::GetName() const {
	return "Present";
}

StageType PresentStage::GetType() const {
	return StageType::Graphics;
}

void PresentStage::Declare(RenderGraphBuilder& b) {
	m_present = b.ImportPresentTarget("Present");
	b.Read(m_input, ResourceUsage::Sampled, "sceneTexture");
	b.Write(m_present, ResourceUsage::ColorAttachment);
	b.SetRenderTarget(m_present);
}

void PresentStage::Compile(RenderGraphContext&) {
}

void PresentStage::BindResources(RenderGraphContext& ctx) {
	ctx.BindInputs(m_mat);
}

void PresentStage::Execute(RenderGraphContext& ctx) {
	DrawRequest req{};
	req.material = m_mat;
	req.mesh = m_quad;
	ctx.GetRenderer()->DrawMesh(req);
}

} // namespace PixieToolbox
