#pragma once
#include <PixieRenderer/RenderGraph/IRenderStage.h>

namespace PixieToolbox {

using namespace PixieRenderer;

class PresentStage : public IRenderStage {
  public:
	PresentStage(std::shared_ptr<IRenderer> renderer, RGResource input);

	std::string_view GetName() const override;
	StageType GetType() const override;

	void Declare(RenderGraphBuilder& b) override;
	void Compile(RenderGraphContext&) override;
	void BindResources(RenderGraphContext& ctx) override;
	void Execute(RenderGraphContext& ctx) override;

  private:
	RGResource m_input, m_present;
	MeshHandle m_quad;
	MaterialHandle m_mat;
};

} // namespace PixieToolbox
