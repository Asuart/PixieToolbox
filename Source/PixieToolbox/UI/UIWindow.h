#pragma once
#include <memory>

namespace PixieRenderer {
class IRenderer;
}

namespace PixieToolbox {

using namespace PixieRenderer;

class UI;

class UIWindow {
  public:
	UIWindow(UI* ui, std::shared_ptr<IRenderer> renderer) : m_ui(ui), m_renderer(renderer) {};
	virtual ~UIWindow() {
	}

	virtual void OnBeforeDraw() {
	}
	virtual void Draw() = 0;

  protected:
	UI* m_ui;
	std::shared_ptr<IRenderer> m_renderer;
};

} // namespace PixieToolbox
