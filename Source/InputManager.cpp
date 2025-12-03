#include "../Header/InputManager.h"
#include <iostream>

InputManager& InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

void InputManager::setupCallbacks(GLFWwindow* window) {
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetKeyCallback(window, keyCallBack);
}

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            getInstance().leftMousePressed = true;
            std::cout << "LMB pressed" << std::endl; // debug
        } 
        else if (action == GLFW_RELEASE) {
            getInstance().leftMousePressed = false;
			std::cout << "LMB released" << std::endl; // debug
        }
    }
}

void InputManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    InputManager& input = getInstance();
    input.lensRadius += static_cast<float>(yoffset) * 10.0f;

    // Clamp radius
    if (input.lensRadius < input.minRadius) input.lensRadius = input.minRadius;
    if (input.lensRadius > input.maxRadius) input.lensRadius = input.maxRadius;

    std::cout << "Lens Radius: " << input.lensRadius << "px" << std::endl;
}

void InputManager::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    InputManager& input = getInstance();
    input.mouseX = xpos;
    input.mouseY = ypos;
}

void InputManager::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void InputManager::update(float deltaTime) {
    // Interpolate lensRevealProgress based on mouse state
    if (leftMousePressed) {
		lensRevealProgress += revealSpeed * deltaTime;
		if (lensRevealProgress > 1.0f) lensRevealProgress = 1.0f;

        std::cout << "LMB Held, Progress: " << lensRevealProgress << std::endl; // debug
    }
    else {
		lensRevealProgress -= revealSpeed * deltaTime;
		if (lensRevealProgress < 0.0f) lensRevealProgress = 0.0f;
    }
}