#include "../Header/Renderer.h"
#include "../Header/Util.h"
#include "../Header/InputManager.h"
#include <iostream>

Renderer::Renderer() :
	m_windowHeight(0),
	m_windowWidth(0),
	m_sceneShader(0),
	m_quadVAO(0),
	m_quadVBO(0),
	m_backgroundTexture(0),
	m_hiddenTexture(0),
	m_projectionLoc(-1) {}

Renderer::~Renderer() {
	cleanup();
}

bool Renderer::initialize(int windowWidth, int windowHeight) {
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	std::cout << "Initializing Renderer..." << std::endl;

	// Shader creation
	m_sceneShader = createShader("Shaders/scene.vert", "Shaders/scene.frag");
	if (m_sceneShader == 0) {
		std::cerr << "Failed to create scene shader!" << std::endl;
		return false;
	}

	m_projectionLoc = glGetUniformLocation(m_sceneShader, "uProjection");

	createQuad();

	setupProjection();

	// Load the background texture (with fallback to placeholder, the ACTUAL texture will be added later, this is just for testing right now)
	m_backgroundTexture = loadImageToTexture("Resources/forest_normal.png"); 

	if (m_backgroundTexture == 0) {
		std::cout << "Failed to load background texture!" << std::endl;
		m_backgroundTexture = createPlaceholderTexture();
	} else {
		std::cout << "Background texture loaded successfully." << std::endl;
	}

	// Load the hidden texture (with fallback to placeholder, the ACTUAL texture will be added later, this is just for testing right now)
	m_hiddenTexture = loadImageToTexture("Resources/forest_hidden.png");
	if (m_hiddenTexture == 0) {
		std::cout << "Failed to load hidden texture, using a placeholder." << std::endl;
		m_hiddenTexture = createHiddenPlaceholderTexture();
	}
	else {
		std::cout << "Hidden texture loaded successfully." << std::endl;
	}

	std::cout << "Renderer initialized successfully." << std::endl;
	return true;
}

unsigned int Renderer::createPlaceholderTexture() {

	// 2x2 gradient texture placeholder (from dark forest green to light green)
	unsigned char pixels[] = {
		25, 38, 30, 255,    // Dark forest green
		40, 60, 45, 255,    // Slightly lighter
		30, 50, 35, 255,    // Medium green
		50, 75, 55, 255     // Lighter green
	};

	unsigned int texture;
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Linear filtering to blend the 2x2 gradient across the screen
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevent wrapping(seams)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << "Placeholder texture created successfully." << std::endl;
	return texture;
}

unsigned int Renderer::createHiddenPlaceholderTexture() {
	unsigned char pixels[] = {
		138, 43, 226, 255,   // Blue-violet (top-left)
		255, 0, 255, 255,    // Magenta (top-right)
		0, 255, 255, 255,    // Cyan (bottom-left)
		255, 105, 180, 255   // Hot pink (bottom-right)
	};

	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << "Hidden placeholder texture created successfully." << std::endl;
	return texture;
}


void Renderer::createQuad() {
	// Fullscreen quad vertices: pos (x, y) + TexCoords (u, v)
	float quadVertices[] = {
		-1.0f, 1.0f, 0.0f, 1.0f, // Top-left
		-1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
		1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right

		-1.0f, 1.0f, 0.0f, 1.0f, // Top-left
		1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right
		1.0f, 1.0f, 1.0f, 1.0f  // Top-right
	};

	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);

	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	std::cout << "Fullscreen quad created succesfully." << std::endl;
}

void Renderer::setupProjection() {
	// Orgraphic projection matrix for fullscreen rendering
	glUseProgram(m_sceneShader);

	float identity[16] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glUniformMatrix4fv(m_projectionLoc, 1, GL_FALSE, identity);
	glUseProgram(0);
}

void Renderer::render(const InputManager& input) {
	glUseProgram(m_sceneShader);

	/// Bind both textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_backgroundTexture);
	glUniform1i(glGetUniformLocation(m_sceneShader, "uTexture"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_hiddenTexture);
	glUniform1i(glGetUniformLocation(m_sceneShader, "uHiddenTexture"), 1);

	// Pass lens uniforms to shader
	glUniform2f(glGetUniformLocation(m_sceneShader, "uMousePos"), static_cast<float>(input.mouseX), static_cast<float>(input.mouseY));
	glUniform1f(glGetUniformLocation(m_sceneShader, "uLensRadius"), input.lensRadius);
	glUniform1f(glGetUniformLocation(m_sceneShader, "uLensRevealProgress"), input.lensRevealProgress);
	glUniform2f(glGetUniformLocation(m_sceneShader, "uWindowSize"), static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));

	// Draw fullscreen quad
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glBindVertexArray(0);
	glUseProgram(0);

}

void Renderer::cleanup() {
	if (m_sceneShader != 0) {
		glDeleteProgram(m_sceneShader);
		m_sceneShader = 0;
	}
	if (m_quadVBO != 0) {
		glDeleteBuffers(1, &m_quadVBO);
		m_quadVBO = 0;
	}
	if (m_quadVAO != 0) {
		glDeleteVertexArrays(1, &m_quadVAO);
		m_quadVAO = 0;
	}
	if (m_backgroundTexture != 0) {
		glDeleteTextures(1, &m_backgroundTexture);
		m_backgroundTexture = 0;
	}

	if (m_hiddenTexture != 0) {
		glDeleteTextures(1, &m_hiddenTexture);
		m_hiddenTexture = 0;
	}

	std::cout << "Renderer resources cleaned up." << std::endl;
}