#pragma once
#include <vector>
#include <memory>

#include <PixieRenderer/Renderer/RenderAPI.h>

#include "Image/UIImage.h"

namespace PixieRenderer {
class IWindow;
class IRenderer;
}

namespace PixieToolbox {

using namespace PixieRenderer;

class UIWindow;

class UI {
  public:
	static std::unique_ptr<UI> Create(IWindow* window, bool docking, RenderAPI api);

	virtual ~UI();

	virtual void AddWindow(UIWindow* window);

	void OnBeforeDrawFrame();
	virtual void Draw() = 0;
	virtual UIImage* CreateUIImage(std::shared_ptr<IRenderer> renderer, FrameBufferHandle handle) = 0;
	virtual UIImage* CreateUIImage(std::shared_ptr<IRenderer> renderer, TextureHandle handle) = 0;

  protected:
	IWindow* m_window;
	std::vector<UIWindow*> m_windows;
	bool m_isDocking;

	UI(IWindow* window, bool docking);

  public:
	template <typename T> std::vector<T*> GetWindowOfType() {
		std::vector<T*> result;
		for (size_t i = 0; i < m_windows.size(); i++) {
			T* cast = dynamic_cast<T*>(m_windows[i]);
			if (cast) {
				result.push_back(cast);
			}
		}
		return result;
	}
};

} // namespace PixieToolbox
