#pragma once
#include <GLFW/glfw3.h>

class InputManager {
public:
	double mouseX = 0.0;
	double mouseY = 0.0;
	bool leftMousePressed = false;

	// Lens properties, starting value, min/max limits
	float lensRadius = 150.0f;
	const float minRadius = 50.0f;
	const float maxRadius = 400.0f;

	// Singleton access
	static InputManager& getInstance();

	void setupCallbacks(GLFWwindow* window);

private:
	InputManager() {}
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
};