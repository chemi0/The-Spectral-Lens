#include "../Header/RunnerMinigame.h"
#include "../Header/InputManager.h"
#include <iostream>
#include <algorithm> 

const float LANE_WIDTH = 2.5f;
const float PLAYER_SPEED = 15.0f;
const float MAX_SURVIVAL_TIME = 20.0f; // Player needs to survive for 20 seconds to win

RunnerMinigame::RunnerMinigame(int width, int height)
	: screenWidth(width), screenHeight(height),
	isGameOver(false), playerWon(false), timeElapsed(0.0f),
	currentLane(1), playerX(0.0f), targetX(0.0f),
	spawnTimer(0.0f), gameSpeed(15.0f), survivalTimer(0.0f),
	camera(glm::vec3(0.0f, 4.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.f, -25.0f) // Camera high behind the player; 3rd person view
{
	// Load Shaders

	shaderProgram = createShader("Shaders/basic_3d.vert", "Shaders/basic_3d.frag");

	// Cube Mesh (For the player, obstacles, floor)
    float vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        // Front face
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        // Left face
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        // Right face
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f
        // Top face
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
}

RunnerMinigame::~RunnerMinigame() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void RunnerMinigame::update(float deltaTime) {
	if (isGameOver) return;

    survivalTimer += deltaTime;
	timeElapsed += deltaTime;

	InputManager& input = InputManager::getInstance();

    bool isRightDown = input.isKeyDown(GLFW_KEY_D) || input.isKeyDown(GLFW_KEY_RIGHT);
    if (isRightDown && !wasRightPressed) {
        if (currentLane < 2) currentLane++;
    }
    wasRightPressed = isRightDown; // Update state for next frame

	bool isLeftDown = input.isKeyDown(GLFW_KEY_A) || input.isKeyDown(GLFW_KEY_LEFT);
    if (isLeftDown && !wasLeftPressed) {
        if (currentLane > 0) currentLane--;
	}
	wasLeftPressed = isLeftDown; // Update state for next frame

	// Target X calculation: Lane 0 = -2.5, Lane 1 = 0.0, Lane 2 = 2.5
    targetX = (currentLane - 1) * LANE_WIDTH;

    // Smooth Interpolation
    if (playerX < targetX) {
		playerX += PLAYER_SPEED * deltaTime;
        if (playerX > targetX) playerX = targetX;
    }
    else if (playerX > targetX) {
        playerX -= PLAYER_SPEED * deltaTime;
		if (playerX < targetX) playerX = targetX;
    }

    // Obstacle Spawning
    spawnTimer -= deltaTime;
    if (spawnTimer <= 0.0f) {
        spawnObstacle();
        
		// Spawn faster over time to increase difficulty
        spawnTimer = 1.5f - (survivalTimer * 0.05f);
        if (spawnTimer < 0.4f) spawnTimer = 0.4f;
    }

    // Obstacle Logic
    float playerZ = 0.0f;
    float collisionMargin = 0.8f;

    for (auto& obs : obstacles) {
		obs.position.z += gameSpeed * deltaTime;

        // Collision Checks
        // Is it the same depth as the player (Z axis)?
        if (obs.position.z > (playerZ - 0.5f) && obs.position.z < (playerZ + 0.5f)) {
            // Is it in the same lane (X axis)?
            if (fabs(playerX - obs.position.x) < collisionMargin) {
                isGameOver = true;
                playerWon = false;
				std::cout << "GAME OVER! You hit an obstacle." << std::endl;
            }
        }
    }
    
	// Remove obstacles that have passed the player (passed the camera)
    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), 
		[](Obstacle& o) { return o.position.z > 5.0f; }), obstacles.end());

    // Win Condition
    if (survivalTimer >= MAX_SURVIVAL_TIME) {
        isGameOver = true;
        playerWon = true;
		std::cout << "VICTORY! You survived the runner minigame." << std::endl;
    }
}

void RunnerMinigame::spawnObstacle() {
    int lane = rand() % 3; // Spawn on a random lane
    float xPos = (lane - 1) * LANE_WIDTH;

    Obstacle obs;
    obs.position = glm::vec3(xPos, 0.0f, -50.0f); // Spawn far ahead
    obs.active = true;
	obstacles.push_back(obs);
}

void RunnerMinigame::render() {
    glUseProgram(shaderProgram);

    // Matrices
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(screenWidth / screenHeight), 0.1f, 100.0f);

    glm::mat4 view = camera.GetViewMatrix();

	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Player [Currently just a green cube, placeholder until the actual model is done]
    glm::mat4 playerModel = glm::mat4(1.0f);
    playerModel = glm::translate(playerModel, glm::vec3(playerX, 0.0f, 0.0f));
	renderCube(playerModel, glm::vec3(0.0f, 1.0f, 0.0f));

    // Obstacles [Red cubes, placeholders for the actual obstacle models]
    for (const auto& obs : obstacles) {
		glm::mat4 obsModel = glm::mat4(1.0f);
        obsModel = glm::translate(obsModel, obs.position);
		renderCube(obsModel, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    // Floor (Grey Strip)
	glm::mat4 floorModel = glm::mat4(1.0f);
    floorModel = glm::translate(floorModel, glm::vec3(0.0f, -1.0f, -25.0f));
    floorModel = glm::scale(floorModel, glm::vec3(LANE_WIDTH * 3 + 2.0f, 0.5f, 60.0f));
	renderCube(floorModel, glm::vec3(0.3f, 0.3f, 0.3f));
}

void RunnerMinigame::renderCube(glm::mat4 model, glm::vec3 color) {
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int colorLoc = glGetUniformLocation(shaderProgram, "uColor");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

bool RunnerMinigame::checkWinCondition() {
    return playerWon;
}

bool RunnerMinigame::isFinished() {
    return isGameOver;
}
