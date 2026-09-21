#include "DemoWindow.h"

#include <imgui.h>

using namespace PixieRenderer;

namespace PixieToolbox {

DemoWindow::DemoWindow(UI* ui, std::shared_ptr<IRenderer> renderer) : UIWindow(ui, renderer) {
}

void DemoWindow::Draw() {
	ImGui::ShowDemoWindow();
}

} // namespace PixieToolbox
