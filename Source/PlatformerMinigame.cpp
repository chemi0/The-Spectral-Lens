#include "../Header/PlatformerMinigame.h"
#include "../Header/InputManager.h"
#include "../Header/Util.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

PlatformerMinigame::PlatformerMinigame(int width, int height)
    : screenWidth(width), screenHeight(height),
    isGameOver(false), playerWon(false), waitingToStart(true), spaceWasPressed(true), timeElapsed(0.0f),
    playerX(0.0f), playerY(0.75f), playerZ(0.0f),  // Standing on starting platform (platform top at 0.25 + PLAYER_HEIGHT/2)
    velocityX(0.0f), velocityY(0.0f), velocityZ(0.0f),
    isGrounded(true), hasDoubleJump(true),  // Start grounded
    playerRotation(0.0f), targetRotation(0.0f),  // Player rotation initialization
    goalCollectRadius(1.5f),
    camera(glm::vec3(0.0f, 8.0f, 14.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -30.0f),  // Higher camera with steeper angle for better platform visibility, the platforms are still a bit weird but oh well
    hudShaderProgram(0), hudVAO(0), hudVBO(0), fontTexture(0)
{
    // Load shader
    shaderProgram = createShader("Shaders/basic_3d.vert", "Shaders/basic_3d.frag");

    // Cube vertices (for platforms)
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Load player model
    if (!playerModel.loadModel("Resources/RunnerCharacter/cat_with_lights.obj")) {
        std::cerr << "Failed to load character model!" << std::endl;
    }

    // Load textures
    playerModel.diffuseTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_diffuse.png");
    playerModel.emissionTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_Emissive.png");
    playerModel.roughnessTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_Roughness.png");
    playerModel.aoTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_AmbientOcclusion.png");
    playerModel.opacityTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_Opacity.png");
    playerModel.normalMapTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_Normal.png");

    // Initialize the level
    initializePlatformMap();
    
    // Initialize HUD for "Press Space to Start"
    initializeHUD();

    std::cout << "Platformer Minigame Initialized!" << std::endl;
    std::cout << "Press SPACE to start!" << std::endl;
    std::cout << "Controls: WASD = Move, SPACE = Jump (Double Jump available!)" << std::endl;
}

PlatformerMinigame::~PlatformerMinigame() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    // Clean up HUD resources
    if (hudVAO != 0) glDeleteVertexArrays(1, &hudVAO);
    if (hudVBO != 0) glDeleteBuffers(1, &hudVBO);
    if (hudShaderProgram != 0) glDeleteProgram(hudShaderProgram);
    if (fontTexture != 0) glDeleteTextures(1, &fontTexture);
}

void PlatformerMinigame::initializeHUD() {
    // Load HUD shader
    hudShaderProgram = createShader("Shaders/hud.vert", "Shaders/hud.frag");
    
    // Load font texture
    fontTexture = loadImageToTexture("Resources/font_texture.png");
    if (fontTexture == 0) {
        std::cerr << "Failed to load font texture for HUD!" << std::endl;
    }
    
    // Create quad VAO for HUD rendering
    float quadVertices[] = {
        // Position   // TexCoord
        0.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 0.0f,   1.0f, 0.0f,
        1.0f, 1.0f,   1.0f, 1.0f,
        
        0.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 1.0f,   1.0f, 1.0f,
        0.0f, 1.0f,   0.0f, 1.0f
    };
    
    glGenVertexArrays(1, &hudVAO);
    glGenBuffers(1, &hudVBO);
    glBindVertexArray(hudVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hudVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void PlatformerMinigame::initializePlatformMap() {
    platforms.clear();

    // Starting platform (larger, safe area)
    Platform start;
    start.position = glm::vec3(0.0f, 0.0f, 0.0f);
    start.size = glm::vec3(5.0f, 0.5f, 5.0f);
    start.type = PlatformType::SOLID;
    start.active = true;
    platforms.push_back(start);

    // Platform 2 - Small jump right
    Platform p2;
    p2.position = glm::vec3(5.0f, 1.0f, 0.0f);
    p2.size = glm::vec3(2.5f, 0.5f, 2.5f);
    p2.type = PlatformType::SOLID;
    p2.active = true;
    platforms.push_back(p2);

    // Platform 3 - Continue right, higher
    Platform p3;
    p3.position = glm::vec3(9.0f, 2.5f, 0.0f);
    p3.size = glm::vec3(2.5f, 0.5f, 2.5f);
    p3.type = PlatformType::SOLID;
    p3.active = true;
    platforms.push_back(p3);

    // Platform 4 - Jump forward (use W key)
    Platform p4;
    p4.position = glm::vec3(9.0f, 3.5f, -5.0f);
    p4.size = glm::vec3(2.5f, 0.5f, 2.5f);
    p4.type = PlatformType::SOLID;
    p4.active = true;
    platforms.push_back(p4);

    // Platform 5 - Continue forward
    Platform p5;
    p5.position = glm::vec3(9.0f, 4.5f, -10.0f);
    p5.size = glm::vec3(2.0f, 0.5f, 2.0f);
    p5.type = PlatformType::SOLID;
    p5.active = true;
    platforms.push_back(p5);

    // Platform 6 - Jump left
    Platform p6;
    p6.position = glm::vec3(5.0f, 5.5f, -10.0f);
    p6.size = glm::vec3(2.5f, 0.5f, 2.5f);
    p6.type = PlatformType::SOLID;
    p6.active = true;
    platforms.push_back(p6);

    // Platform 7 - Continue left and back
    Platform p7;
    p7.position = glm::vec3(1.0f, 6.5f, -8.0f);
    p7.size = glm::vec3(2.5f, 0.5f, 2.5f);
    p7.type = PlatformType::SOLID;
    p7.active = true;
    platforms.push_back(p7);

    // Moving platform - moves in Z axis
    Platform moving1;
    moving1.position = glm::vec3(1.0f, 7.5f, -12.0f);
    moving1.size = glm::vec3(2.5f, 0.5f, 2.5f);
    moving1.type = PlatformType::MOVING;
    moving1.active = true;
    moving1.moveStart = glm::vec3(1.0f, 7.5f, -12.0f);
    moving1.moveEnd = glm::vec3(1.0f, 7.5f, -18.0f);
    moving1.moveSpeed = 3.0f;
    moving1.movingForward = true;
    platforms.push_back(moving1);

    // Platform 8 - Jump right and forward
    Platform p8;
    p8.position = glm::vec3(6.0f, 8.5f, -18.0f);
    p8.size = glm::vec3(2.0f, 0.5f, 2.0f);
    p8.type = PlatformType::SOLID;
    p8.active = true;
    platforms.push_back(p8);

    // Small stepping stones
    Platform step1;
    step1.position = glm::vec3(9.0f, 9.0f, -20.0f);
    step1.size = glm::vec3(1.5f, 0.5f, 1.5f);
    step1.type = PlatformType::SOLID;
    step1.active = true;
    platforms.push_back(step1);

    Platform step2;
    step2.position = glm::vec3(6.0f, 9.5f, -23.0f);
    step2.size = glm::vec3(1.5f, 0.5f, 1.5f);
    step2.type = PlatformType::SOLID;
    step2.active = true;
    platforms.push_back(step2);

    // Anotehr moving paltform
    Platform moving2;
    moving2.position = glm::vec3(3.0f, 10.0f, -26.0f);
    moving2.size = glm::vec3(2.5f, 0.5f, 2.5f);
    moving2.type = PlatformType::MOVING;
    moving2.active = true;
    moving2.moveStart = glm::vec3(3.0f, 10.0f, -26.0f);
    moving2.moveEnd = glm::vec3(10.0f, 10.0f, -26.0f);
    moving2.moveSpeed = 2.5f;
    moving2.movingForward = true;
    platforms.push_back(moving2);

    // Platform going back
    Platform p9;
    p9.position = glm::vec3(12.0f, 11.0f, -22.0f);
    p9.size = glm::vec3(2.5f, 0.5f, 2.5f);
    p9.type = PlatformType::SOLID;
    p9.active = true;
    platforms.push_back(p9);

    // Platform 10 - more back
    Platform p10;
    p10.position = glm::vec3(12.0f, 12.0f, -17.0f);
    p10.size = glm::vec3(2.0f, 0.5f, 2.0f);
    p10.type = PlatformType::SOLID;
    p10.active = true;
    platforms.push_back(p10);

    // Platform 11 - Jump left to final
    Platform p11;
    p11.position = glm::vec3(7.0f, 13.0f, -15.0f);
    p11.size = glm::vec3(2.5f, 0.5f, 2.5f);
    p11.type = PlatformType::SOLID;
    p11.active = true;
    platforms.push_back(p11);

    // FInal platform with the goal
    Platform finalPlat;
    finalPlat.position = glm::vec3(2.0f, 14.0f, -15.0f);
    finalPlat.size = glm::vec3(4.0f, 0.5f, 4.0f);
    finalPlat.type = PlatformType::SOLID;
    finalPlat.active = true;
    platforms.push_back(finalPlat);

    // Goal position (floating above final platform)
    goalPosition = glm::vec3(2.0f, 16.0f, -15.0f);
}

void PlatformerMinigame::update(float deltaTime) {
    if (isGameOver) return;

    InputManager& input = InputManager::getInstance();
    bool spaceIsPressed = input.isKeyDown(GLFW_KEY_SPACE);
    
    // Always update camera to follow player, even while waiting
    // Camera is higher and behind player for better platform visibility, idk where else to put it to make the platforms look better for the player
    camera.Position = glm::vec3(playerX, playerY + 8.0f, playerZ + 16.0f);
    
    // "Space to Start" - more of a bugfix than a feature
    if (waitingToStart) {
        // Update time for pulsing animation even while waiting
        timeElapsed += deltaTime;
        
        // Wait for space key press (edge detection - must release first then press)
        if (!spaceIsPressed) {
            spaceWasPressed = false;
        }
        if (spaceIsPressed && !spaceWasPressed) {
            waitingToStart = false;
            spaceWasPressed = true;
            std::cout << "Game Started!" << std::endl;
        }
        return; // Don't update physics while waiting
    }

    timeElapsed += deltaTime;

    // Horizontal movement (WASD)
    float moveInputX = 0.0f;
    float moveInputZ = 0.0f;

    if (input.isKeyDown(GLFW_KEY_A)) moveInputX -= 1.0f;
    if (input.isKeyDown(GLFW_KEY_D)) moveInputX += 1.0f;
    if (input.isKeyDown(GLFW_KEY_W)) moveInputZ -= 1.0f;
    if (input.isKeyDown(GLFW_KEY_S)) moveInputZ += 1.0f;

    velocityX = moveInputX * MOVE_SPEED;
    velocityZ = moveInputZ * MOVE_SPEED;

    // Update player rotation based on movement direction
    if (moveInputX != 0.0f || moveInputZ != 0.0f) {
        // Calculate target rotation angle based on movement direction
        // atan2(x, z) gives the angle from the movement vector
        // Note: Z is negative when moving forward (W key), positive when moving backward (S key)
        targetRotation = atan2(moveInputX, moveInputZ);
    }
    
    // Smoothly interpolate current rotation toward target rotation
    float rotationDiff = targetRotation - playerRotation;
    
    // Handle angle wrapping (ensure we take the shortest path)
    while (rotationDiff > 3.14159f) rotationDiff -= 2.0f * 3.14159f;
    while (rotationDiff < -3.14159f) rotationDiff += 2.0f * 3.14159f;
    
    // Apply smooth rotation
    if (fabs(rotationDiff) > 0.01f) {
        playerRotation += rotationDiff * ROTATION_SPEED * deltaTime;
    } else {
        playerRotation = targetRotation;
    }

    // Jumping (edge detection using member variable)
    if (spaceIsPressed && !spaceWasPressed) {
        if (isGrounded) {
            velocityY = JUMP_FORCE;
            isGrounded = false;
            hasDoubleJump = true;
            std::cout << "Jump!" << std::endl;
        } else if (hasDoubleJump) {
            velocityY = DOUBLE_JUMP_FORCE;
            hasDoubleJump = false;
            std::cout << "Double Jump!" << std::endl;
        }
    }
    spaceWasPressed = spaceIsPressed;

    // Apply gravity
    velocityY += GRAVITY * deltaTime;

    // Update position
    playerX += velocityX * deltaTime;
    playerY += velocityY * deltaTime;
    playerZ += velocityZ * deltaTime;

    // Update moving platforms
    updateMovingPlatforms(deltaTime);

    // Check collisions
    checkCollisions();

    // Check for goal
    float distToGoal = glm::length(glm::vec3(playerX, playerY, playerZ) - goalPosition);
    if (distToGoal < goalCollectRadius) {
        isGameOver = true;
        playerWon = true;
        std::cout << "VICTORY! Goal collected!" << std::endl;
    }

    // Fall death
    if (playerY < -10.0f) {
        isGameOver = true;
        playerWon = false;
        std::cout << "GAME OVER! You fell off!" << std::endl;
    }
}

void PlatformerMinigame::updateMovingPlatforms(float deltaTime) {
    for (auto& platform : platforms) {
        if (platform.type == PlatformType::MOVING) {
            if (platform.movingForward) {
                platform.position += glm::normalize(platform.moveEnd - platform.moveStart) * platform.moveSpeed * deltaTime;
                if (glm::length(platform.position - platform.moveEnd) < 0.1f) {
                    platform.movingForward = false;
                }
            } else {
                platform.position += glm::normalize(platform.moveStart - platform.moveEnd) * platform.moveSpeed * deltaTime;
                if (glm::length(platform.position - platform.moveStart) < 0.1f) {
                    platform.movingForward = true;
                }
            }
        }
    }
}

void PlatformerMinigame::checkCollisions() {
    isGrounded = false;

    for (const auto& platform : platforms) {
        glm::vec3 normal;
        if (checkPlatformCollision(platform, normal)) {
            // If collision from above (landing on platform)
            if (normal.y > 0.5f && velocityY < 0.0f) {
                playerY = platform.position.y + platform.size.y * 0.5f + PLAYER_HEIGHT * 0.5f;
                velocityY = 0.0f;
                isGrounded = true;
                hasDoubleJump = true;
            }
        }
    }
}

bool PlatformerMinigame::checkPlatformCollision(const Platform& platform, glm::vec3& normal) {
    // Simple AABB collision
    glm::vec3 playerMin = glm::vec3(playerX - PLAYER_RADIUS, playerY - PLAYER_HEIGHT * 0.5f, playerZ - PLAYER_RADIUS);
    glm::vec3 playerMax = glm::vec3(playerX + PLAYER_RADIUS, playerY + PLAYER_HEIGHT * 0.5f, playerZ + PLAYER_RADIUS);

    glm::vec3 platformMin = platform.position - platform.size * 0.5f;
    glm::vec3 platformMax = platform.position + platform.size * 0.5f;

    bool collision = (playerMin.x <= platformMax.x && playerMax.x >= platformMin.x) &&
                     (playerMin.y <= platformMax.y && playerMax.y >= platformMin.y) &&
                     (playerMin.z <= platformMax.z && playerMax.z >= platformMin.z);

    if (collision) {
        normal = glm::vec3(0.0f, 1.0f, 0.0f); // Top surface normal
    }

    return collision;
}

void PlatformerMinigame::render() {
    glUseProgram(shaderProgram);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(camera.Position));

    // Render platforms
    for (const auto& platform : platforms) {
        renderPlatform(platform);
    }

    // Render goal (golden cube)
    glm::mat4 goalModel = glm::mat4(1.0f);
    goalModel = glm::translate(goalModel, goalPosition);
    goalModel = glm::rotate(goalModel, timeElapsed, glm::vec3(0.0f, 1.0f, 0.0f));
    goalModel = glm::scale(goalModel, glm::vec3(1.0f));
    renderCube(goalModel, glm::vec3(1.0f, 0.84f, 0.0f)); // Gold color

    // Render player with rotation
    glm::mat4 playerMatrix = glm::mat4(1.0f);
    playerMatrix = glm::translate(playerMatrix, glm::vec3(playerX, playerY, playerZ));
    // Apply player rotation around Y axis (facing direction)
    playerMatrix = glm::rotate(playerMatrix, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    playerMatrix = glm::scale(playerMatrix, glm::vec3(0.005f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(playerMatrix));
    
    // Set texture uniforms
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);
    
    playerModel.render();
    
    // Render "Press Space to Start" indicator if waiting
    if (waitingToStart) {
        renderStartPrompt();
    }
}

void PlatformerMinigame::renderStartPrompt() {
    // Render a pulsing indicator cube above the player to signal "Press Space"
    glUseProgram(shaderProgram);
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    // Pulsing effect
    float pulse = 0.5f + 0.5f * sin(timeElapsed * 4.0f);
    float scale = 0.3f + 0.1f * pulse;
    
    // Floating indicator above player
    glm::mat4 indicatorModel = glm::mat4(1.0f);
    indicatorModel = glm::translate(indicatorModel, glm::vec3(playerX, playerY + 2.5f + 0.3f * sin(timeElapsed * 2.0f), playerZ));
    indicatorModel = glm::rotate(indicatorModel, timeElapsed * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    indicatorModel = glm::scale(indicatorModel, glm::vec3(scale));
    
    // Pulsing cyan color for visibility
    glm::vec3 indicatorColor = glm::vec3(0.0f, pulse, 1.0f);
    renderCube(indicatorModel, indicatorColor);
    
    // Render a second indicator - an arrow pointing down
    glm::mat4 arrowModel = glm::mat4(1.0f);
    arrowModel = glm::translate(arrowModel, glm::vec3(playerX, playerY + 1.8f + 0.1f * sin(timeElapsed * 3.0f), playerZ));
    arrowModel = glm::scale(arrowModel, glm::vec3(0.15f, 0.4f, 0.15f));
    renderCube(arrowModel, glm::vec3(1.0f, 1.0f, 1.0f)); // White arrow
}

void PlatformerMinigame::renderPlatform(const Platform& platform) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, platform.position);
    model = glm::scale(model, platform.size);

    glm::vec3 color;
    if (platform.type == PlatformType::MOVING) {
        color = glm::vec3(0.5f, 0.5f, 1.0f); // Blue for moving
    } else {
        color = glm::vec3(0.3f, 0.7f, 0.3f); // Green for solid
    }

    renderCube(model, color);
}

void PlatformerMinigame::renderCube(glm::mat4 model, glm::vec3 color) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, glm::value_ptr(color));
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0);

    glBindVertexArray(VAO);
    glDisableVertexAttribArray(3);
    glVertexAttrib3f(3, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

bool PlatformerMinigame::checkWinCondition() {
    return playerWon;
}

bool PlatformerMinigame::isFinished() {
    return isGameOver;
}
