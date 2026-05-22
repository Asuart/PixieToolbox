#pragma once
#include <vector>

class MainWindow;
class UIElement;

union SDL_Event;

class WindowUI {
public:
	WindowUI(MainWindow* window, bool docking);
	virtual ~WindowUI();

	virtual void HandleEvent(const SDL_Event& event);
	virtual void AddElement(UIElement* element);

	virtual void Draw() = 0;

protected:
	MainWindow* m_window;
	std::vector<UIElement*> m_elements;
	bool m_isDocking;

public:
	template<typename T>
	std::vector<T*> GetElementOfType() {
		std::vector<T*> result;
		for (size_t i = 0; i < m_elements.size(); i++) {
			T* cast = dynamic_cast<T*>(m_elements[i]);
			if (cast) {
				result.push_back(cast);
			}
		}
		return result;
	}
};
