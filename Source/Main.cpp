#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "../Header/Util.h"
#include "../Header/InputManager.h"
#include "../Header/Renderer.h"

// Main fajl funkcija sa osnovnim komponentama OpenGL programa

// Projekat je dozvoljeno pisati počevši od ovog kostura
// Toplo se preporučuje razdvajanje koda po fajlovima (i eventualno potfolderima) !!!
// Srećan rad!
int main()
{
	if (!glfwInit()) return endProgram("GLFW couldn't initialize.");
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Primary monitor for fullscreen
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	// Create fullscreen window
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "The Spectral Lens", primaryMonitor, NULL);
	if (window == NULL) {
		return endProgram("Window couldn't be created.");
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		return endProgram("GLEW couldn't initialize.");
	}

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Resolution: " << mode->width << "x" << mode->height << std::endl;

	// Input handling
	InputManager& input = InputManager::getInstance();
	input.setupCallbacks(window);

	// Renderer setup
	Renderer renderer;
	if (!renderer.initialize(mode->width, mode->height)) {
		return endProgram("Renderer couldn't initialize.");
	}

	// OpenGL state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.1f, 0.15f, 0.12f, 1.0f);

	// FPS cap @ 75FPS
	const double targetFrameTime = 1.0 / 75.0;
	double lastFrameTime = glfwGetTime();
	double deltaTime = 0.0;

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		double frameStartTime = glfwGetTime();
		deltaTime = frameStartTime - lastFrameTime;
		lastFrameTime = frameStartTime;

		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Render the scene
		renderer.render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		// FPS cap
		double frameEndTime = glfwGetTime();
		double frameDuration = frameEndTime - frameStartTime;
		if (frameDuration < targetFrameTime) {
			while (glfwGetTime() < frameEndTime + (targetFrameTime - frameDuration)) {
				// Wait to cap the framerate
			}
		}
	}

	renderer.cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}