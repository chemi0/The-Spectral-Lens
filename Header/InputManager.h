#pragma once
#include <GLFW/glfw3.h>

class InputManager {
public:
	double mouseX = 0.0;
	double mouseY = 0.0;
	bool leftMousePressed = false;
	bool findKeyPressed = false; // 'E' key for finding entities

	// Lens properties, starting value, min/max limits
	float lensRadius = 150.0f;
	const float minRadius = 50.0f;
	const float maxRadius = 400.0f;

	// Lens animation
	float lensRevealProgress = 0.0f; // 0.0 -> fully hidden, 1.0 -> fully revealed
	const float revealSpeed = 1.0f; // speed multiplier (1.0 = 1 second to fully reveal)

	// Singleton access
	static InputManager& getInstance();

	// Setup callbacks
	void setupCallbacks(GLFWwindow* window);

	// Update animation state (called every frame)
	void update(float deltaTime);

private:
	InputManager() = default;

	// Callback functions
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
};