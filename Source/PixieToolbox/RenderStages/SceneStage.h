#pragma once
#include <PixieRenderer/RenderGraph/IRenderStage.h>

namespace PixieToolbox {

using namespace PixieRenderer;

class Scene;

struct CameraUBO {
	glm::mat4 view;
	glm::mat4 projection;
};

class SceneStage : public IRenderStage {
  public:
	SceneStage(std::shared_ptr<IRenderer> renderer, RGResource output, glm::uvec2 res);

	std::string_view GetName() const override;
	StageType GetType() const override;

	void Declare(RenderGraphBuilder& b) override;
	void Compile(RenderGraphContext&) override;
	void BindResources(RenderGraphContext& ctx) override;
	void Execute(RenderGraphContext& ctx) override;

  private:
	std::unique_ptr<Scene> m_scene;
	RGResource m_output;
	BufferHandle m_cameraUBO;
	BufferHandle m_cameraPositionUBO;
	glm::uvec2 m_resolution;
};

} // namespace PixieToolbox
