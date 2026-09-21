#pragma once
#include "PixieToolbox/UI/UIWindow.h"

namespace PixieToolbox {

class DemoWindow : public UIWindow {
  public:
	DemoWindow(UI* ui, std::shared_ptr<IRenderer> renderer);

	void Draw() override;
};

} // namespace PixieToolbox
