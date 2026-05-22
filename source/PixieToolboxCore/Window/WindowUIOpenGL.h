#pragma once
#include "WindowUI.h"

class MainWindowOpenGL;

class WindowUIOpenGL : public WindowUI {
public:
	WindowUIOpenGL(MainWindowOpenGL* window, bool docking);
	~WindowUIOpenGL();

	void Draw();
};
