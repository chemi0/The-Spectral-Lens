#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class InputManager;

class Renderer {
public:
	Renderer();
	~Renderer();

	// Initialize renderer (load shaders, set up buffers, etc.)
	bool initialize(int windowWidth, int windowHeight);

	// Render the scene
	void render(const InputManager& input);

	// Cleanup resources
	void cleanup();

	unsigned int createPlaceholderTexture();

private:
	// Window dimensions
	int m_windowWidth;
	int m_windowHeight;

	// Shader program IDs
	unsigned int m_sceneShader;

	// Fullscreen quad VAO/VBO
	unsigned int m_quadVAO;
	unsigned int m_quadVBO;

	// Textures
	unsigned int m_backgroundTexture;
	unsigned int m_hiddenTexture;

	// Projection matrix location
	int m_projectionLoc;

	// Helper function to create fullscreen quad
	void createQuad();

	// Helper function to setup orthographic projection
	void setupProjection();

	unsigned int createHiddenPlaceholderTexture();
};