#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Header/Util.h"
#include "../Header/InputManager.h"
#include "../Header/Renderer.h"
#include "../Header/GameStateManager.h"

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

	// Custom cursor setup
	GLFWcursor* customCursor = loadImageToCursor("Resources/cursor_lens1.png");
	if (customCursor != nullptr) {
		glfwSetCursor(window, customCursor);
		std::cout << "Custom cursor loaded successfully." << std::endl;
	} 
	else {
		std::cout << "Failed to load custom cursor." << std::endl;
	}

	// Input handling
	InputManager& input = InputManager::getInstance();
	input.setupCallbacks(window);

	// Renderer setup
	Renderer renderer;
	if (!renderer.initialize(mode->width, mode->height)) {
		return endProgram("Renderer couldn't initialize.");
	}

	// Game State Manager setup
	GameStateManager& gameManager = GameStateManager::getInstance();
	gameManager.setRenderer(&renderer);

	// OpenGL state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable depth testing and face culling by default (can be toggled with T and C keys)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glClearColor(0.1f, 0.15f, 0.12f, 1.0f);

	std::cout << "\n=== CONTROLS ===" << std::endl;
	std::cout << "T - Toggle Depth Testing (currently: ENABLED)" << std::endl;
	std::cout << "C - Toggle Face Culling (currently: ENABLED)" << std::endl;
	std::cout << "ESC - Exit application" << std::endl;
	std::cout << "================\n" << std::endl;

	// FPS cap @ 75FPS
	const double targetFrameTime = 1.0 / 75.0;
	double lastFrameTime = glfwGetTime();
	double deltaTime = 0.0f;

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		double frameStartTime = glfwGetTime();
		deltaTime = frameStartTime - lastFrameTime;
		lastFrameTime = frameStartTime;

		// Update input animation state
		input.update(static_cast<float>(deltaTime));

		// 2D/3D Logic split
		if (gameManager.getState() == GameState::LENS_2D) {

			if (input.findKeyPressed && input.lensRevealProgress > 0.5f) {
				Entity* clickedEntity = renderer.checkEntityClick(
					static_cast<float>(input.mouseX),
					static_cast<float>(input.mouseY),
					input.lensRadius,
					input.lensRevealProgress);

				if (clickedEntity != nullptr) {
					gameManager.startMinigame(clickedEntity->movement, clickedEntity);
				}
				input.findKeyPressed = false; // preventing multiple triggers
			}
			if (input.resetKeyPressed) {
				renderer.resetEntities();
			}

			// Update 2D Animations
			renderer.update(static_cast<float>(deltaTime), input);
		} else {
			// 3D-specific input handling will be here later
		}

		gameManager.update(static_cast<float>(deltaTime));

		gameManager.render(input);

		glfwSwapBuffers(window);
		glfwPollEvents();


		//// Clear screen
		//glClear(GL_COLOR_BUFFER_BIT);

		//renderer.update(deltaTime, input);

		//// Render the scene
		//renderer.render(input);

		//glfwSwapBuffers(window);
		//glfwPollEvents();

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