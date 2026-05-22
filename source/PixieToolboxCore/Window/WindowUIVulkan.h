#pragma once
#include "WindowUI.h"

class MainWindowVulkan;

class WindowUIVulkan: public WindowUI {
public:
	WindowUIVulkan(MainWindowVulkan* window, bool docking);
	~WindowUIVulkan();

	void Draw();
};
