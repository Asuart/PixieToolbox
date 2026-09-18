#pragma once
#include <vector>

#include <PixieRenderer/Renderer/IRenderer.h>

#include "Image/UIImage.h"

namespace PixieToolbox {

using namespace PixieRenderer;

class UIWindow;

class UI {
  public:
	UI(IWindow* window, bool docking);
	virtual ~UI();

	virtual void AddWindow(UIWindow* window);

	void OnBeforeDrawFrame();
	virtual void Draw() = 0;
	virtual UIImage* CreateUIImage(
	    IRenderer* renderer,
	    FrameBufferHandle handle
	) = 0;
	virtual UIImage* CreateUIImage(
	    IRenderer* renderer,
	    TextureHandle handle
	) = 0;

  protected:
	IWindow* m_window;
	std::vector<UIWindow*> m_windows;
	bool m_isDocking;

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
