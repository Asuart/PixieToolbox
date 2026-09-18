#pragma once
#include <memory>

#include <GLFW/glfw3.h>

#include <PixieRenderer/Renderer/IRenderer.h>
#include <PixieRenderer/Renderer/RenderAPI.h>

namespace PixieToolbox {

using namespace PixieRenderer;

class ImGuiBackend {
  public:
	virtual ~ImGuiBackend() = default;
	virtual void Init(GLFWwindow* w, IRenderer* r) = 0;
	virtual void Shutdown() = 0;
	virtual void NewFrame() = 0;
	virtual void Render() = 0;

	static std::unique_ptr<ImGuiBackend> Create(RenderAPI api);
};

} // namespace PixieToolbox
