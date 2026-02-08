#pragma once
#include <GLFW/glfw3.h>

class InputManager {
public:
	double mouseX = 0.0;
	double mouseY = 0.0;
	bool leftMousePressed = false;
	bool rightMousePressed = false;
	bool findKeyPressed = false; // 'E' key for finding entities
	bool resetKeyPressed = false; // 'R' key to reset 

	// Lens properties, starting value, min/max limits
	float lensRadius = 150.0f;
	const float minRadius = 50.0f;
	const float maxRadius = 400.0f;

	// Lens animation
	float lensRevealProgress = 0.0f; // 0.0 -> fully hidden, 1.0 -> fully revealed
	const float revealSpeed = 1.0f; // speed multiplier (1.0 = 1 second to fully reveal)

	bool keys[1024]; // Array to store all key states

	// Rendering toggles (global)
	bool depthTestEnabled = true;   // T key to toggle
	bool faceCullingEnabled = true; // C key to toggle

	// Helper function to check any key
	bool isKeyDown(int key) {
		if (key >= 0 && key < 1024) return keys[key];
		return false;
	}

	// Singleton access
	static InputManager& getInstance();

	// Setup callbacks
	void setupCallbacks(GLFWwindow* window);

	// Update animation state (called every frame)
	void update(float deltaTime);

private:
	InputManager() {
		for (int i = 0; i < 1024; i++) keys[i] = false;
	}

	// Callback functions
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
};