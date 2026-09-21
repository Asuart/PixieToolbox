#pragma once
#include <PixieRenderer/RenderGraph/IRenderStage.h>

namespace PixieToolbox {

using namespace PixieRenderer;

class BlurStage : public IRenderStage {
  public:
	BlurStage(std::shared_ptr<IRenderer> renderer, RGResource in, RGResource out, glm::uvec2 size);

	std::string_view GetName() const override;
	StageType GetType() const override;

	void Declare(RenderGraphBuilder& b) override;
	void Compile(RenderGraphContext&) override;
	void BindResources(RenderGraphContext& ctx) override;
	void Execute(RenderGraphContext& ctx) override;

  private:
	RGResource m_input, m_output;
	ComputeProgramHandle m_prog;
	glm::uvec2 m_size;
};

} // namespace PixieToolbox
