#include "../Header/Renderer.h"
#include "../Header/Util.h"
#include "../Header/InputManager.h"
#include <iostream>

Renderer::Renderer() :
	m_windowHeight(0),
	m_windowWidth(0),
	m_sceneShader(0),
	m_entityShader(0),
	m_hudShader(0),
	m_quadVAO(0),
	m_quadVBO(0),
	m_entityVAO(0),
	m_entityVBO(0),
	m_hudVAO(0),
	m_hudVBO(0),
	m_backgroundTexture(0),
	m_hiddenTexture(0),
	m_snowTexture(0),
	m_moonTexture(0),
	m_fontTexture(0),
	m_projectionLoc(-1) {}

Renderer::~Renderer() {
	cleanup();
}

bool Renderer::initialize(int windowWidth, int windowHeight) {
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	std::cout << "Initializing Renderer..." << std::endl;

	// Shader shader
	m_sceneShader = createShader("Shaders/scene.vert", "Shaders/scene.frag");
	if (m_sceneShader == 0) {
		std::cerr << "Failed to create scene shader!" << std::endl;
		return false;
	}
	m_projectionLoc = glGetUniformLocation(m_sceneShader, "uProjection");

	// Entity shader
	m_entityShader = createShader("Shaders/entity.vert", "Shaders/entity.frag");
	if (m_entityShader ==0) {
		std::cerr << "Failed to create entity shader!" << std::endl;
		return false;
	}

	// HUD shader
	m_hudShader = createShader("Shaders/hud.vert", "Shaders/hud.frag");
	if (m_hudShader == 0) {
		std::cerr << "Failed to create HUD shader!" << std::endl;
		return false;
	}

	// Snow texture
	m_snowTexture = loadImageToTexture("Resources/snow.png");
	if (m_snowTexture == 0) {
		std::cout << "Failed to load snow texture!" << std::endl;
	}
	else {
		std::cout << "Snow texcre loaded successfully." << std::endl;
	}

	// Allow the wrapping of the snow texture for tiling
	glBindTexture(GL_TEXTURE_2D, m_snowTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Moon texture
	m_moonTexture = loadImageToTexture("Resources/Moon2.png");
	if (m_moonTexture == 0) {
		std::cout << "Failed to load moon texture!" << std::endl;
	}
	else {
		std::cout << "Moon texture loaded successfully." << std::endl;
	}

	// Font texture
	m_fontTexture = loadImageToTexture("Resources/font_texture.png");
	if (m_fontTexture == 0) {
		std::cout << "Failed to load font texture!" << std::endl;
		m_fontTexture = createFontTexture();
	}
	else {
		std::cout << "Font texture loaded successfully." << std::endl;
	}

	createQuad();
	createEntityQuad();
	createHUDQuad();
	setupProjection();

	// Main and secret background textures (with fallback to placeholders, in case the .pngs get corrupted somehow)
	m_backgroundTexture = loadImageToTexture("Resources/main_background_v1.png"); 
	
	if (m_backgroundTexture == 0) {
		std::cout << "Failed to load background texture!" << std::endl;
		m_backgroundTexture = createMainTexture();
	} else {
		std::cout << "Background texture loaded successfully." << std::endl;
	}

	m_hiddenTexture = loadImageToTexture("Resources/hidden_forest.png");
	if (m_hiddenTexture == 0) {
		std::cout << "Failed to load hidden texture, using a placeholder." << std::endl;
		m_hiddenTexture = createHiddenTexture();
	}
	else {
		std::cout << "Hidden texture loaded successfully." << std::endl;
	}

	initializeEntities();

	std::cout << "Renderer initialized successfully." << std::endl;
	return true;
}



void Renderer::initializeEntities() {
	Entity witchCat;
	witchCat.posX = 1.0f;
	witchCat.posY = 0.6f;
	witchCat.width = 0.15f;
	witchCat.height = 0.12f;
	witchCat.speed = 0.12f;
	witchCat.texture = loadImageToTexture("Resources/witch_cat.png");
	witchCat.movement = MovementType::HorizontalLoop;
	witchCat.found = false;

	if (witchCat.texture != 0) {
		m_entities.push_back(witchCat);
		std::cout << "Witch cat entity initialized successfully." << std::endl;
	}
	else {
		std::cout << "Failed to load witch cat texture!" << std::endl;
	}

	// I think 4 more entities will suffice for the demo, add here
}

void Renderer::update(float deltaTime, const InputManager& input) {
	updateEntities(deltaTime);
}

void Renderer::updateEntities(float deltaTime) {
	for (auto& entity : m_entities) {
		// if (entity.found) continue; // don't animate found entities

		switch (entity.movement) {
		case MovementType::HorizontalLoop: {
			entity.posX -= entity.speed * deltaTime;

			// Wrap around when off-screen (left)
			if (entity.posX < -entity.width) {
				entity.posX = 1.0f;
			}

			float bobAmount = 0.03f;
			float bobSpeed = 2.0f;
			entity.posY = 0.6f + static_cast<float>(sin(glfwGetTime() * bobSpeed) * bobAmount);

			break;
		}
		case MovementType::Bounce:
			break;

		case MovementType::Static:
		default:
			break;
		}
	}
}

unsigned int Renderer::createMainTexture() {

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

unsigned int Renderer::createHiddenTexture() {
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

void Renderer::createEntityQuad() {

	float entityVertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f, // Top-left
		0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
		1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
		0.0f, 1.0f, 0.0f, 1.0f, // Top-left
		1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
		1.0f, 1.0f, 1.0f, 1.0f  // Top-right
	};

	glGenVertexArrays(1, &m_entityVAO);
	glGenBuffers(1, &m_entityVBO);

	glBindVertexArray(m_entityVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_entityVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(entityVertices), entityVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	std::cout << "Entity quad created successfully." << std::endl;
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

	// Bind snow texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_snowTexture);
	glUniform1i(glGetUniformLocation(m_sceneShader, "uSnowTexture"), 2);

	// Bind moon texture

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_moonTexture);
	glUniform1i(glGetUniformLocation(m_sceneShader, "uMoonTexture"), 3);


	// Pass lens uniforms to shader
	glUniform2f(glGetUniformLocation(m_sceneShader, "uMousePos"), static_cast<float>(input.mouseX), static_cast<float>(input.mouseY));
	glUniform1f(glGetUniformLocation(m_sceneShader, "uLensRadius"), input.lensRadius);
	glUniform1f(glGetUniformLocation(m_sceneShader, "uLensRevealProgress"), input.lensRevealProgress);
	glUniform2f(glGetUniformLocation(m_sceneShader, "uWindowSize"), static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));

	// Animation passtime
	glUniform1f(glGetUniformLocation(m_sceneShader, "uTime"), static_cast<float>(glfwGetTime()));

	// Draw fullscreen quad
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glBindVertexArray(0);
	glUseProgram(0);


	// Render entities on top
	renderEntities(input);

	renderHUD();
}

void Renderer::renderEntities(const InputManager& input) {
	glUseProgram(m_entityShader);

	// Enable blending for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Pass lens uniforms
	glUniform2f(glGetUniformLocation(m_entityShader, "uMousePos"),
		static_cast<float>(input.mouseX), static_cast<float>(input.mouseY));
	glUniform1f(glGetUniformLocation(m_entityShader, "uLensRadius"), input.lensRadius);
	glUniform1f(glGetUniformLocation(m_entityShader, "uLensRevealProgress"), input.lensRevealProgress);
	glUniform2f(glGetUniformLocation(m_entityShader, "uWindowSize"),
		static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));

	glBindVertexArray(m_entityVAO);

	for (const auto& entity : m_entities) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, entity.texture);
		glUniform1i(glGetUniformLocation(m_entityShader, "uEntityTexture"), 0);

		glUniform2f(glGetUniformLocation(m_entityShader, "uEntityPos"), entity.posX, entity.posY);
		glUniform2f(glGetUniformLocation(m_entityShader, "uEntitySize"), entity.width, entity.height);

		glUniform1f(glGetUniformLocation(m_entityShader, "uIsFound"), entity.found ? 1.0f : 0.0f);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glBindVertexArray(0);
	glDisable(GL_BLEND);
	glUseProgram(0);
}

bool Renderer::checkEntityClick(float mouseX, float mouseY, float lensRadius, float revealProgress) {
	if (revealProgress <= 0.5f) return false;

	// Convert mouse pos to UV space
	float uvX = mouseX / static_cast<float>(m_windowWidth);
	float uvY = 1.0f - (mouseY / static_cast<float>(m_windowHeight));

	for (auto& entity : m_entities) {
		if (entity.found) continue;

		float entityCenterX = entity.posX + entity.width * 0.5f;
		float entityCenterY = entity.posY + entity.height * 0.5f;

		// Check if mouse is within entity bounds
		if (uvX >= entity.posX && uvX <= (entity.posX + entity.width) &&
			uvY >= entity.posY && uvY <= (entity.posY + entity.height)) {

			float entityScreenX = entityCenterX * static_cast<float>(m_windowWidth);
			float entityScreenY = (1.0f - entityCenterY) * static_cast<float>(m_windowHeight);

			float distToLens = sqrt((entityScreenX - mouseX) * (entityScreenX - mouseX) +
									(entityScreenY - mouseY) * (entityScreenY - mouseY));

			if (distToLens <= lensRadius) {
				entity.found = true;
				std::cout << "Entity found!" << std::endl;
				return true;
			}
		}
	}
	return false;
}

void Renderer::createHUDQuad() {
	float hudVertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f, // Top-left
		0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
		1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
		0.0f, 1.0f, 0.0f, 1.0f, // Top-left
		1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
		1.0f, 1.0f, 1.0f, 1.0f  // Top-right
	};

	glGenVertexArrays(1, &m_hudVAO);
	glGenBuffers(1, &m_hudVBO);

	glBindVertexArray(m_hudVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_hudVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hudVertices), hudVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	std::cout << "HUD quad created successfully." << std::endl;

}

unsigned int Renderer::createFontTexture() {
	// Extended font atlas:
	// 0123456789/ABCDEFGHIJKLMNOPQRSTUVWXYZ (total 39 characters)
	const int charWidth = 8;
	const int charHeight = 8;
	const int numChars = 39;
	const int atlasWidth = charWidth * numChars;
	const int atlasHeight = charHeight;

	unsigned char pixels[atlasWidth * atlasHeight];
	memset(pixels, 0, sizeof(pixels));

	// Bitmap patter for digits 0-9 and '/'; Each digit is represented as an 8x8 grid of 1s and 0s
	const unsigned char charPatterns[39][8] = {
		// 0 - 9
		{0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00}, // 0
		{0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00}, // 1
		{0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00}, // 2
		{0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00}, // 3
		{0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00}, // 4
		{0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00}, // 5
		{0x1C, 0x30, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00}, // 6
		{0x7E, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x00}, // 7
		{0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00}, // 8 
		{0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00}, // 9

		// '/'
		{0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00}, // '/' (index 10)

		// A-Z
		{0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00}, // A (index 11)
		{0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00}, // B
		{0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00}, // C
		{0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00}, // D
		{0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00}, // E
		{0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00}, // F
		{0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00}, // G
		{0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00}, // H
		{0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00}, // I
		{0x3E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00}, // J
		{0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00}, // K
		{0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00}, // L
		{0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00}, // M
		{0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00}, // N
		{0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}, // O
		{0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00}, // P
		{0x3C, 0x66, 0x66, 0x66, 0x6A, 0x6C, 0x36, 0x00}, // Q
		{0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00}, // R
		{0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00}, // S
		{0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00}, // T
		{0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}, // U
		{0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00}, // V
		{0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // W
		{0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00}, // X
		{0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00}, // Y
		{0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00}, // Z (index 36)

		// : (colon)
		{0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00}, // : (index 37)

		// space (blank)
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // space (index 38)

	};

	for (int charIdx = 0; charIdx < numChars; charIdx++) {
		for (int row = 0; row < charHeight; row++) {
			unsigned char rowPattern = charPatterns[charIdx][row];
			for (int col = 0; col < charWidth; col++) {
				int pixelX = charIdx * charWidth + col;
				int pixelY = row;

				// Check if bit is set 
				bool isSet = (rowPattern >> (7 - col)) & 0x01;
				pixels[pixelY * atlasWidth + pixelX] = isSet ? 255 : 0;
			}
		}
	}

	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << "Extended font texture created successfully." << std::endl;
	return texture;
}

int Renderer::getFoundCount() const {
	int count = 0;
	for (const auto& entity : m_entities) {
		if (entity.found) count++;
	}
	return count;
}

int Renderer::getTotalCount() const {
	return static_cast<int>(m_entities.size());
}

void Renderer::renderText(const std::string& text, float x, float y, float scale) {
	/* Character mapping :
		'0' - '9' : indices 0-9
		'/'       : index 10
		'A' - 'Z' : indices 11-36
		':' : index 37
		' ' (space): index 38
	*/
	const int numChars = 39;
	const float charWidth = 1.0f / static_cast<float>(numChars); // Width per character in atlas

	glBindVertexArray(m_hudVAO);

	float cursorX = x;
	for (char c : text) {
		int charIndex = -1;

		if (c >= '0' && c <= '9') {
			charIndex = c - '0';
		}
		else if (c == '/') {
			charIndex = 10;
		}
		else if (c >= 'A' && c <= 'Z') {
			charIndex = 11 + (c - 'A');
		}
		else if (c >= 'a' && c <= 'z') {
			// Convert lowercase to uppercase
			charIndex = 11 + (c - 'a');
		}
		else if (c == ':') {
			charIndex = 37;
		}
		else if (c == ' ') {
			charIndex = 38;
		}
		else {
			// Skip unsupported characters
			continue;
		}

		float uvLeft = charIndex * charWidth;
		float uvRight = (charIndex + 1) * charWidth;

		// Update VBO with correct UVs for this character
		float charVertices[] = {
			// pos      // tex
			0.0f, 1.0f, uvLeft,  0.0f, // Top-left
			0.0f, 0.0f, uvLeft,  1.0f, // Bottom-left
			1.0f, 0.0f, uvRight, 1.0f, // Bottom-right
			0.0f, 1.0f, uvLeft,  0.0f, // Top-left
			1.0f, 0.0f, uvRight, 1.0f, // Bottom-right
			1.0f, 1.0f, uvRight, 0.0f  // Top-right
		};

		glBindBuffer(GL_ARRAY_BUFFER, m_hudVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(charVertices), charVertices);

		// Set character position and size uniforms
		glUniform2f(glGetUniformLocation(m_hudShader, "uCharPos"), cursorX, y);
		glUniform2f(glGetUniformLocation(m_hudShader, "uCharSize"), scale, scale * 1.5f); // Slightly taller than wide

		glDrawArrays(GL_TRIANGLES, 0, 6);

		cursorX += scale; // Move cursor for next character
	}

	glBindVertexArray(0);
}

void Renderer::renderHUD() {
	glUseProgram(m_hudShader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fontTexture);
	glUniform1i(glGetUniformLocation(m_hudShader, "uFontTexture"), 0);

	// White text color
	glUniform4f(glGetUniformLocation(m_hudShader, "uTextColor"), 1.0f, 1.0f, 1.0f, 1.0f);

	// Build the "Entity Counter" string: "X/Y"
	int found = getFoundCount();
	int total = getTotalCount();
	std::string counterText = "Entities found: " + std::to_string(found) + "/" + std::to_string(total);

	// Position in bottom-right corner (NDC coordinates)
	// x: right side minus some padding and text width estimate
	// y: bottom plus some padding
	float charScale = 0.05f; // Size of each character in NDC
	float textWidth = counterText.length() * charScale;
	float xPos = 1.0f - textWidth - 0.05f; // Right-aligned with padding
	float yPos = -0.95f; // Near bottom

	renderText(counterText, xPos, yPos, charScale);

	glUniform4f(glGetUniformLocation(m_hudShader, "uTextColor"), 1.0f, 1.0f, 1.0f, 1.0f);

	std::string watermark = "Milan Spasojevic RA147/2022";
	float wmScale = 0.03f;
	float wmX = -0.98f;
	float wmY = -0.95f;

	renderText(watermark, wmX, wmY, wmScale);

	glDisable(GL_BLEND);
	glUseProgram(0);
}


void Renderer::cleanup() {
	if (m_sceneShader != 0) {
		glDeleteProgram(m_sceneShader);
		m_sceneShader = 0;
	}

	if (m_entityShader != 0) {
		glDeleteProgram(m_entityShader);
		m_entityShader = 0;
	}

	if (m_hudShader != 0) {
		glDeleteProgram(m_hudShader);
		m_hudShader = 0;
	}

	if (m_quadVBO != 0) {
		glDeleteBuffers(1, &m_quadVBO);
		m_quadVBO = 0;
	}

	if (m_quadVAO != 0) {
		glDeleteVertexArrays(1, &m_quadVAO);
		m_quadVAO = 0;
	}

	if (m_entityVBO != 0) {
		glDeleteBuffers(1, &m_entityVBO);
		m_entityVBO = 0;
	}

	if (m_entityVAO != 0) {
		glDeleteVertexArrays(1, &m_entityVAO);
		m_entityVAO = 0;
	}

	if (m_hudVBO != 0) {
		glDeleteBuffers(1, &m_hudVBO);
		m_hudVBO = 0;
	}

	if (m_hudVAO != 0) {
		glDeleteVertexArrays(1, &m_hudVAO);
		m_hudVAO = 0;
	}

	if (m_backgroundTexture != 0) {
		glDeleteTextures(1, &m_backgroundTexture);
		m_backgroundTexture = 0;
	}

	if (m_hiddenTexture != 0) {
		glDeleteTextures(1, &m_hiddenTexture);
		m_hiddenTexture = 0;
	}

	if (m_moonTexture != 0) {
		glDeleteTextures(1, &m_moonTexture);
		m_moonTexture = 0;
	}	

	if (m_snowTexture != 0) {
		glDeleteTextures(1, &m_snowTexture);
		m_snowTexture = 0;
	}	

	if (m_fontTexture != 0) {
		glDeleteTextures(1, &m_fontTexture);
		m_fontTexture = 0;
	}

	for (auto& entity : m_entities) {
		if (entity.texture != 0) {
			glDeleteTextures(1, &entity.texture);
			entity.texture = 0;
		}
	}
	m_entities.clear();

	std::cout << "Renderer resources cleaned up." << std::endl;
}