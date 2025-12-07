#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Entity.h"
#include <string>

class InputManager;

class Renderer {
public:
	Renderer();
	~Renderer();

	// Initialize renderer (load shaders, set up buffers, etc.)
	bool initialize(int windowWidth, int windowHeight);

	// Render the scene
	void render(const InputManager& input);

	// Update entity positions
	void update(float deltaTime, const InputManager& input);

	// Cleanup resources
	void cleanup();

	bool checkEntityClick(float mouseX, float mouseY, float lensRadius, float revealProgress);

	// Reset 
	void resetEntities();

	// Discovery counts
	int getFoundCount() const;
	int getTotalCount() const;

private:
	// Window dimensions
	int m_windowWidth;
	int m_windowHeight;

	// Shader program IDs
	unsigned int m_sceneShader;
	unsigned int m_entityShader;
	unsigned int m_hudShader;
	unsigned int m_bushShader;

	// Fullscreen quad VAO/VBO
	unsigned int m_quadVAO;
	unsigned int m_quadVBO;

	// Entity rendering VAO/VBO
	unsigned int m_entityVAO;
	unsigned int m_entityVBO;

	// HUD rendering VAO/VBO
	unsigned int m_hudVAO;
	unsigned int m_hudVBO;

	// Textures
	unsigned int m_backgroundTexture;
	unsigned int m_hiddenTexture;
	unsigned int m_snowTexture;
	unsigned int m_moonTexture;
	unsigned int m_fontTexture;
	unsigned int m_bushTexture;

	std::vector<Entity> m_entities;

	// Projection matrix location
	int m_projectionLoc;

	// Helper function
	void createQuad();
	void createEntityQuad();
	void createHUDQuad();

	void setupProjection();

	void initializeEntities();

	void renderEntities(const InputManager& input);
	void updateEntities(float deltaTime);
	void renderHUD();
	void renderText(const std::string& text, float x, float y, float scale);
	void renderBushForeground();

	unsigned int createMainTexture();
	unsigned int createHiddenTexture();
	unsigned int createFontTexture();

};