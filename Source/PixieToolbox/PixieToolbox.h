#pragma once
#include <filesystem>
#include <string>

#include <glm/glm.hpp>

#include <PixieRenderer/Renderer/RenderAPI.h>

namespace PixieRenderer {
class IRenderer;
class IWindow;
class RenderGraph;
}; // namespace PixieRenderer

namespace PixieToolbox {

using namespace PixieRenderer;

class UI;
class Scene;

class PixieToolboxApp {
  public:
	PixieToolboxApp(const std::string& name, glm::uvec2 resolution, RenderAPI api);
	~PixieToolboxApp();

	void Start();

	void LoadScene(const std::filesystem::path& path);

  private:
	std::unique_ptr<IWindow> m_window = nullptr;
	std::unique_ptr<RenderGraph> m_renderGraph = nullptr;
	std::unique_ptr<UI> m_ui = nullptr;
	std::shared_ptr<IRenderer> m_renderer = nullptr;
	std::shared_ptr<Scene> m_scene = nullptr;

	std::string m_pendingDropFile;

	void UpdateRenderGraph();
};

} // namespace PixieToolbox
