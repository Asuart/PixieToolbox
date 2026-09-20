#pragma once
#include "PixieToolbox/UI/UIWindow.h"

namespace PixieToolbox {

class DemoWindow : public UIWindow {
  public:
	DemoWindow(UI* ui, PixieRenderer::IRenderer* renderer);

	void Draw() override;
};

} // namespace PixieToolbox
