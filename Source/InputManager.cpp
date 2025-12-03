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
        getInstance().leftMousePressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
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