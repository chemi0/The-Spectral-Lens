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
    verticalVelocity(0.0f), isJumping(false), playerScaleY(1.0f), targetScaleY(1.0f),
	camera(glm::vec3(0.0f, 4.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.f, -25.0f) // Camera high behind the player; 3rd person view
{
	// Load Shaders

	shaderProgram = createShader("Shaders/basic_3d.vert", "Shaders/basic_3d.frag");

	// Cube Mesh (For the player, obstacles, floor)
    float vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

         // Bottom face
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

         // Top face
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribute 1: UV (Texture Coords)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Attribute 2: Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    if (!playerModel.loadModel("Resources/RunnerCharacter/cat_with_lights.obj")) {
        std::cerr << "Failed to load character model!" << std::endl;
        return;
    }

    // Verify textures loaded (0 means failed to load)
    playerModel.diffuseTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_diffuse.png");
    if (playerModel.diffuseTex == 0) std::cerr << "Failed to load diffuse texture!" << std::endl;

    playerModel.emissionTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_Emissive.png");
    if (playerModel.emissionTex == 0) std::cerr << "Failed to load emission texture!" << std::endl;

    playerModel.roughnessTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_Roughness.png");
    if (playerModel.roughnessTex == 0) std::cerr << "Failed to load roughness texture!" << std::endl;

    playerModel.aoTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_AmbientOcclusion.png");
    if (playerModel.aoTex == 0) std::cerr << "Failed to load AO texture!" << std::endl;

    playerModel.opacityTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_Opacity.png");
    if (playerModel.opacityTex == 0) std::cerr << "Failed to load opacity texture!" << std::endl;
	
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

    // Vertical Movement (Jumping/Ducking)
    // Ducking (Hold CTRL)
    if (input.isKeyDown(GLFW_KEY_LEFT_CONTROL) || input.isKeyDown(GLFW_KEY_RIGHT_CONTROL)) {
        targetScaleY = 0.5f; // Crouch down
    } else {
        targetScaleY = 1.0f; // Stand up
	}

	// Smooth Ducking Transition (Interplolate Scale)
    if (playerScaleY > targetScaleY) {
        playerScaleY -= DUCK_SPEED * deltaTime;
		if (playerScaleY < targetScaleY) playerScaleY = targetScaleY;
    }
    else if (playerScaleY < targetScaleY) {
		playerScaleY += DUCK_SPEED * deltaTime;
		if (playerScaleY > targetScaleY) playerScaleY = targetScaleY;
    }

    // Jumping (Press Space)
	if (input.isKeyDown(GLFW_KEY_SPACE) && !isJumping && playerY <= 0.05f) { // <0.05f allows a tiny margin of error
        verticalVelocity = JUMP_FORCE;
        isJumping = true;
	}

    // Gravity
	playerY += verticalVelocity * deltaTime;

    if (playerY > 0.0f || verticalVelocity > 0.0f) {
        verticalVelocity += GRAVITY * deltaTime;
    }
        // Ground Check
    if (playerY <= 0.0f) {
        playerY = 0.0f;
        verticalVelocity = 0.0f;
        isJumping = false;
    }
    // Update the Visuals/Camera following later


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

    for (auto& obs : obstacles) {
		obs.position.z += gameSpeed * deltaTime;

        // Collision Checks
        // Z Overlap Check (Depth)
        if (obs.position.z > -0.4f && obs.position.z < 0.4f) {

            // X (Lane) Overlap Check
            if (fabs(playerX - obs.position.x) < 0.6f) {

                bool hit = false;
                
                if (obs.type == GROUND) {
                    // If player feelt (playerY) are lower than the obstacle top (~0.9f) it is registered as a hit
                    if (playerY < 0.5f) hit = true;
                }
                else if (obs.type == FLYING_LOW) {
                    // If player head (scaled height) hits obstacle bottom it is registered as a hit;
                    // Flying Obstacle is at Y = 1.0f, Player usually Y = 1.0f; if scale is > 0.7f, register a hit
                    if (playerScaleY > 0.7f) hit = true;
                }

                if (hit) {
                    isGameOver = true;
                    playerWon = false;
                    std::cout << "GAME OVER! You hit an obstacle." << std::endl;
                }
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

    obs.active = true;

    // 40% chance for a flying obstacle
    if (rand() % 100 < 40) {
        obs.type = FLYING_LOW;

		// Flying obstacle at head level (Y = 1.0f)
        obs.position = glm::vec3(xPos, 1.0f, -50.0f);
    }
    else {
        obs.type = GROUND;

		// Ground obstacle at foot level (Y = 0.0f)
        obs.position = glm::vec3(xPos, 0.0f, -50.0f);
    }

	obstacles.push_back(obs);
}

void RunnerMinigame::render() {
    glUseProgram(shaderProgram);

    // Matrices
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(screenWidth / screenHeight), 0.1f, 100.0f);

    glm::mat4 view = camera.GetViewMatrix();

	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
	unsigned int camPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniform3fv(camPosLoc, 1, glm::value_ptr(camera.Position));

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(camPosLoc, 1, glm::value_ptr(camera.Position));

    // Calculate Player Matrix
    glm::mat4 playerMatrix = glm::mat4(1.0f);

    // World Position
    playerMatrix = glm::translate(playerMatrix, glm::vec3(playerX, playerY, 0.0f));

    // Global Scale (Scaledown of the Model)
    playerMatrix = glm::scale(playerMatrix, glm::vec3(0.005f));

    // Rotation
    playerMatrix = glm::rotate(playerMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Ducking animation
    float centerOffset = (1.0f - playerScaleY) * -0.5f;
    playerMatrix = glm::translate(playerMatrix, glm::vec3(0.0f, centerOffset * 20.0f, 0.0f));
    playerMatrix = glm::scale(playerMatrix, glm::vec3(1.0f, playerScaleY, 1.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(playerMatrix));

    // Texture binding before rendering (fix)
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, playerModel.diffuseTex);
    //glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);

    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, playerModel.emissionTex);
    //glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);

    //glActiveTexture(GL_TEXTURE2);
    //glBindTexture(GL_TEXTURE_2D, playerModel.roughnessTex);
    //glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);

    //glActiveTexture(GL_TEXTURE3);
    //glBindTexture(GL_TEXTURE_2D, playerModel.aoTex);
    //glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);

    //glActiveTexture(GL_TEXTURE4);
    //glBindTexture(GL_TEXTURE_2D, playerModel.opacityTex);
    //glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);

    //glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);

    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);

    // Draw
    playerModel.render();

    // Obstacles [Red cubes, placeholders for the actual obstacle models]
    for (const auto& obs : obstacles) {
		glm::mat4 obsModel = glm::mat4(1.0f);
        obsModel = glm::translate(obsModel, obs.position);

        glm::vec3 color;
        if (obs.type == GROUND) color = glm::vec3(1.0f, 0.0f, 0.0f); // Red for ground obstacles;
		else color = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow for flying obstacles

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
    unsigned int useTexLoc = glGetUniformLocation(shaderProgram, "useTexture");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));
    glUniform1i(useTexLoc, 0); // False (0) = Use Solid Color

    glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

bool RunnerMinigame::checkWinCondition() {
    return playerWon;
}

bool RunnerMinigame::isFinished() {
    return isGameOver;
}
