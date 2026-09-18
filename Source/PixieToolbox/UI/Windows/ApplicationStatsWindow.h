#pragma once
#include "PixieUIApplication/UIWindow.h"

namespace PixieToolbox {

class ApplicationStatsWindow : public UIWindow {
  public:
	ApplicationStatsWindow(UI* ui, PixieRenderer::IRenderer* renderer);

	void Draw() override;
};

} // namespace PixieToolbox
