#pragma once
#include "PixieToolbox/UI/UIWindow.h"

namespace PixieToolbox {

class ApplicationStatsWindow : public UIWindow {
  public:
	ApplicationStatsWindow(UI* ui, std::shared_ptr<IRenderer> renderer);

	void Draw() override;
};

} // namespace PixieToolbox
