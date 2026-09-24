#include "UserInput.h"

#include <GLFW/glfw3.h>

namespace PixieToolbox {

void UserInput::Initialize(GLFWwindow* window) {
	s_window = window;
	if (!window) {
		return;
	}

	s_prevKeyCallback = glfwSetKeyCallback(window, KeyCallback);
	s_prevMouseButtonCallback = glfwSetMouseButtonCallback(window, MouseButtonCallback);
	s_prevCursorPosCallback = glfwSetCursorPosCallback(window, CursorPosCallback);
	s_prevScrollCallback = glfwSetScrollCallback(window, ScrollCallback);
	s_prevWindowFocusCallback = glfwSetWindowFocusCallback(window, WindowFocusCallback);

	s_keyDown.fill(0);
	s_keyPressed.fill(0);
	s_keyReleased.fill(0);
	s_mouseDown.fill(0);
	s_mousePressed.fill(0);
	s_mouseReleased.fill(0);
	s_mousePos = glm::dvec2(0.0);
	s_mouseDelta = glm::dvec2(0.0);
	s_mouseScroll = glm::dvec2(0.0);
	s_firstMouse = true;
	s_cursorCaptured = false;
	s_hasFocus = true;
}

void UserInput::Shutdown() {
	if (s_window) {
		glfwSetKeyCallback(s_window, s_prevKeyCallback);
		glfwSetMouseButtonCallback(s_window, s_prevMouseButtonCallback);
		glfwSetCursorPosCallback(s_window, s_prevCursorPosCallback);
		glfwSetScrollCallback(s_window, s_prevScrollCallback);
		glfwSetWindowFocusCallback(s_window, s_prevWindowFocusCallback);
	}
	s_window = nullptr;
	s_prevKeyCallback = nullptr;
	s_prevMouseButtonCallback = nullptr;
	s_prevCursorPosCallback = nullptr;
	s_prevScrollCallback = nullptr;
	s_prevWindowFocusCallback = nullptr;
}

void UserInput::Reset() {
	s_keyPressed.fill(0);
	s_keyReleased.fill(0);
	s_mousePressed.fill(0);
	s_mouseReleased.fill(0);
	s_mouseDelta = glm::dvec2(0.0);
	s_mouseScroll = glm::dvec2(0.0);
}

bool UserInput::IsKeyDown(int key) {
	return key >= 0 && static_cast<size_t>(key) < s_keyDown.size() && s_keyDown[key] != 0;
}

bool UserInput::IsKeyPressed(int key) {
	return key >= 0 && static_cast<size_t>(key) < s_keyPressed.size() && s_keyPressed[key] != 0;
}

bool UserInput::IsKeyReleased(int key) {
	return key >= 0 && static_cast<size_t>(key) < s_keyReleased.size() && s_keyReleased[key] != 0;
}

bool UserInput::IsMouseButtonDown(int b) {
	return b >= 0 && static_cast<size_t>(b) < s_mouseDown.size() && s_mouseDown[b] != 0;
}

bool UserInput::IsMouseButtonPressed(int b) {
	return b >= 0 && static_cast<size_t>(b) < s_mousePressed.size() && s_mousePressed[b] != 0;
}

bool UserInput::IsMouseButtonReleased(int b) {
	return b >= 0 && static_cast<size_t>(b) < s_mouseReleased.size() && s_mouseReleased[b] != 0;
}

glm::dvec2 UserInput::GetMousePosition() {
	return s_mousePos;
}

glm::dvec2 UserInput::GetMouseDelta() {
	return s_mouseDelta;
}

glm::dvec2 UserInput::GetMouseScroll() {
	return s_mouseScroll;
}

void UserInput::SetCursorCaptured(bool captured) {
	if (s_cursorCaptured == captured) {
		return;
	}
	s_cursorCaptured = captured;

	if (!s_window) {
		return;
	}

	glfwSetInputMode(s_window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

	if (captured && glfwRawMouseMotionSupported()) {
		glfwSetInputMode(s_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	} else {
		glfwSetInputMode(s_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}

	s_firstMouse = true;
}

bool UserInput::IsCursorCaptured() {
	return s_cursorCaptured;
}

void UserInput::KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods) {
	if (s_prevKeyCallback) {
		s_prevKeyCallback(w, key, scancode, action, mods);
	}

	if (key < 0 || static_cast<size_t>(key) >= s_keyDown.size()) {
		return;
	}
	switch (action) {
	case GLFW_PRESS:
		s_keyDown[key] = 1;
		s_keyPressed[key] = 1;
		break;
	case GLFW_RELEASE:
		s_keyDown[key] = 0;
		s_keyReleased[key] = 1;
		break;
	case GLFW_REPEAT:
		break;
	default:
		break;
	}
}

void UserInput::MouseButtonCallback(GLFWwindow* w, int button, int action, int mods) {
	if (s_prevMouseButtonCallback) {
		s_prevMouseButtonCallback(w, button, action, mods);
	}

	if (button < 0 || static_cast<size_t>(button) >= s_mouseDown.size()) {
		return;
	}
	if (action == GLFW_PRESS) {
		s_mouseDown[button] = 1;
		s_mousePressed[button] = 1;
	} else if (action == GLFW_RELEASE) {
		s_mouseDown[button] = 0;
		s_mouseReleased[button] = 1;
	}
}

void UserInput::CursorPosCallback(GLFWwindow* w, double x, double y) {
	if (s_prevCursorPosCallback) {
		s_prevCursorPosCallback(w, x, y);
	}

	const glm::dvec2 newPos(x, y);
	if (s_firstMouse) {
		s_mousePos = newPos;
		s_firstMouse = false;
		return;
	}
	s_mouseDelta += newPos - s_mousePos;
	s_mousePos = newPos;
}


void UserInput::ScrollCallback(GLFWwindow* w, double xoff, double yoff) {
	if (s_prevScrollCallback) {
		s_prevScrollCallback(w, xoff, yoff);
	}

	s_mouseScroll += glm::dvec2(xoff, yoff);
}

void UserInput::WindowFocusCallback(GLFWwindow* w, int focused) {
	if (s_prevWindowFocusCallback) {
		s_prevWindowFocusCallback(w, focused);
	}

	s_hasFocus = (focused == GLFW_TRUE);
	if (!s_hasFocus) {
		s_keyDown.fill(0);
		s_mouseDown.fill(0);
	}
}

} // namespace PixieToolbox
