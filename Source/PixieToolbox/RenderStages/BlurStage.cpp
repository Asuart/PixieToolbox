#include "BlurStage.h"

#include <PixieRenderer/ComputeProgram/BlurComputeProgram.h>
#include <PixieRenderer/RenderGraph/RenderGraphBuilder.h>
#include <PixieRenderer/RenderGraph/RenderGraphContext.h>
#include <PixieRenderer/Renderer/IRenderer.h>

namespace PixieToolbox {

BlurStage::BlurStage(std::shared_ptr<IRenderer> renderer, RGResource in, RGResource out, glm::uvec2 size)
    : IRenderStage(renderer), m_input(in), m_output(out), m_size(size) {
	BlurComputeProgram blurProgram;
	m_prog = m_renderer->CreateComputeProgram(&blurProgram);
}

std::string_view BlurStage::GetName() const {
	return "Blur";
}

StageType BlurStage::GetType() const {
	return StageType::Compute;
}

void BlurStage::Declare(RenderGraphBuilder& b) {
	b.Read(m_input, ResourceUsage::StorageRead, "inputImage");
	b.Write(m_output, ResourceUsage::StorageWrite, "outputImage");
}

void BlurStage::Compile(RenderGraphContext&) {
}

void BlurStage::BindResources(RenderGraphContext& ctx) {
	ctx.BindInputs(m_prog);
}

void BlurStage::Execute(RenderGraphContext& ctx) {
	struct Push {
		int32_t radius;
		int32_t p0, p1, p2;
	} pc{};
	pc.radius = 2;
	DispatchRequest req{};
	req.program = m_prog;
	req.x = (m_size.x + 7) / 8;
	req.y = (m_size.y + 7) / 8;
	req.z = 1;
	req.inlineData = std::as_bytes(std::span{ &pc, 1 });
	ctx.GetRenderer()->DispatchComputeProgram(req);
}

} // namespace PixieToolbox
