#pragma once
#include "UI.h"

namespace PixieToolbox {

class UIOpenGL : public UI {
  public:
	UIOpenGL(IWindow* window, bool docking);
	~UIOpenGL();

	void Draw();
	UIImage* CreateUIImage(std::shared_ptr<IRenderer> renderer, FrameBufferHandle handle) override;
	UIImage* CreateUIImage(std::shared_ptr<IRenderer> renderer, TextureHandle handle) override;
};

} // namespace PixieToolbox
