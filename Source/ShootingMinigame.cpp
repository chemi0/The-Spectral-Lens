#include "../Header/ShootingMinigame.h"
#include "../Header/InputManager.h"
#include "../Header/Util.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ShootingMinigame::ShootingMinigame(int width, int height)
    : screenWidth(width), screenHeight(height),
    isGameOver(false), playerWon(false), waitingToStart(true), spaceWasPressed(true),
    timeElapsed(0.0f), gameTimer(0.0f),
    showingName(false), nameDisplayTimer(0.0f),
    playerPos(0.0f, 3.0f, 0.0f),
    playerYaw(-90.0f), playerPitch(20.0f),
    lastMouseX(width / 2.0f), lastMouseY(height / 2.0f), firstMouse(true),
    cursorLocked(false),
    isCharging(false), chargeTime(0.0f),
    camera(glm::vec3(0.0f, PLAYER_HEIGHT, 0.0f)),
    hudShaderProgram(0), hudVAO(0), hudVBO(0), fontTexture(0),
    nameTexture(0), nameVAO(0), nameVBO(0)
{
    srand(static_cast<unsigned int>(time(nullptr)));

    shaderProgram = createShader("Shaders/basic_3d.vert", "Shaders/basic_3d.frag");

    // Cube vertices (for HUD elements and ground fallback)
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

    // Load cat model (same as platformer/hidden game)
    if (!catModel.loadModel("Resources/RunnerCharacter/cat_with_lights.obj")) {
        std::cerr << "Failed to load cat model for ShootingMinigame!" << std::endl;
    }
    catModel.diffuseTex = catModel.loadTexture("Resources/RunnerCharacter/cat_diffuse.png");
    catModel.emissionTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Emissive.png");
    catModel.roughnessTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Roughness.png");
    catModel.aoTex = catModel.loadTexture("Resources/RunnerCharacter/cat_AmbientOcclusion.png");
    catModel.opacityTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Opacity.png");
    catModel.normalMapTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Normal.png");

    // Load pumpkin projectile model (same as runner obstacle)
    if (!pumpkinModel.loadModel("Resources/RunnerObstacle/obstacle.obj")) {
        std::cerr << "Failed to load pumpkin model for ShootingMinigame!" << std::endl;
    }
    pumpkinModel.diffuseTex = pumpkinModel.loadTexture("Resources/RunnerObstacle/obstacleBase.png");
    pumpkinModel.aoTex = pumpkinModel.loadTexture("Resources/RunnerObstacle/obstacleAO.png");
    pumpkinModel.normalMapTex = pumpkinModel.loadTexture("Resources/RunnerObstacle/obstacleNormal.png");
    pumpkinModel.roughnessTex = pumpkinModel.loadTexture("Resources/RunnerObstacle/obstacleRoughness.png");

    // Load slingshot model
    if (!slingshotModel.loadModel("Resources/Slingshot/slingshot.obj")) {
        std::cerr << "Failed to load slingshot model for ShootingMinigame!" << std::endl;
    }
    // Slingshot textures
    slingshotModel.diffuseTex = slingshotModel.loadTexture("Resources/Slingshot/Slingshot.png");
    slingshotModel.normalMapTex = slingshotModel.loadTexture("Resources/Slingshot/normalMap1.png");
    slingshotModel.roughnessTex = slingshotModel.loadTexture("Resources/Slingshot/Material.001.png");

    // Load ground platform model (same as runner/hidden game)
    if (!groundModel.loadModel("Resources/PlatformTexture/rock_wall_16_1k.obj")) {
        std::cerr << "Failed to load ground model for ShootingMinigame!" << std::endl;
    }
    groundModel.diffuseTex = groundModel.loadTexture("Resources/PlatformTexture/diffWall.png");
    groundModel.normalMapTex = groundModel.loadTexture("Resources/PlatformTexture/normalWall.png");
    groundModel.aoTex = groundModel.loadTexture("Resources/PlatformTexture/armWall.png");

    // Load skybox texture (reuse platformer skybox)
    skyboxTexture = loadImageToTexture("Resources/SkyboxCubemaps/HidingCubemap.png");
    if (skyboxTexture == 0) {
        std::cerr << "Failed to load skybox texture for ShootingMinigame!" << std::endl;
    }

    // Create skybox cube VAO
    float skyboxVertices[] = {
        // Back face
        -1.0f,  1.0f, -1.0f,   0.25f, 0.3334f,
        -1.0f, -1.0f, -1.0f,   0.25f, 0.6666f,
         1.0f, -1.0f, -1.0f,   0.50f, 0.6666f,
         1.0f, -1.0f, -1.0f,   0.50f, 0.6666f,
         1.0f,  1.0f, -1.0f,   0.50f, 0.3334f,
        -1.0f,  1.0f, -1.0f,   0.25f, 0.3334f,
        // Left face
        -1.0f, -1.0f,  1.0f,   0.00f, 0.6666f,
        -1.0f, -1.0f, -1.0f,   0.25f, 0.6666f,
        -1.0f,  1.0f, -1.0f,   0.25f, 0.3334f,
        -1.0f,  1.0f, -1.0f,   0.25f, 0.3334f,
        -1.0f,  1.0f,  1.0f,   0.00f, 0.3334f,
        -1.0f, -1.0f,  1.0f,   0.00f, 0.6666f,
        // Right face
         1.0f, -1.0f, -1.0f,   0.50f, 0.6666f,
         1.0f, -1.0f,  1.0f,   0.75f, 0.6666f,
         1.0f,  1.0f,  1.0f,   0.75f, 0.3334f,
         1.0f,  1.0f,  1.0f,   0.75f, 0.3334f,
         1.0f,  1.0f, -1.0f,   0.50f, 0.3334f,
         1.0f, -1.0f, -1.0f,   0.50f, 0.6666f,
        // Front face
        -1.0f, -1.0f,  1.0f,   1.00f, 0.6666f,
        -1.0f,  1.0f,  1.0f,   1.00f, 0.3334f,
         1.0f,  1.0f,  1.0f,   0.75f, 0.3334f,
         1.0f,  1.0f,  1.0f,   0.75f, 0.3334f,
         1.0f, -1.0f,  1.0f,   0.75f, 0.6666f,
        -1.0f, -1.0f,  1.0f,   1.00f, 0.6666f,
        // Top face
        -1.0f,  1.0f, -1.0f,   0.25f, 0.3334f,
         1.0f,  1.0f, -1.0f,   0.50f, 0.3334f,
         1.0f,  1.0f,  1.0f,   0.50f, 0.0000f,
         1.0f,  1.0f,  1.0f,   0.50f, 0.0000f,
        -1.0f,  1.0f,  1.0f,   0.25f, 0.0000f,
        -1.0f,  1.0f, -1.0f,   0.25f, 0.3334f,
        // Bottom face
        -1.0f, -1.0f, -1.0f,   0.25f, 0.6666f,
        -1.0f, -1.0f,  1.0f,   0.25f, 1.0000f,
         1.0f, -1.0f, -1.0f,   0.50f, 0.6666f,
         1.0f, -1.0f, -1.0f,   0.50f, 0.6666f,
        -1.0f, -1.0f,  1.0f,   0.25f, 1.0000f,
         1.0f, -1.0f,  1.0f,   0.50f, 1.0000f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Initialize the scene
    initializeScene();
    initializeHUD();

    std::cout << "Shooting Minigame Initialized!" << std::endl;
    std::cout << "Press SPACE to start!" << std::endl;
    std::cout << "Controls: Mouse = Aim, Left Click (hold) = Charge, Release = Fire" << std::endl;
}

ShootingMinigame::~ShootingMinigame() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    if (hudVAO != 0) glDeleteVertexArrays(1, &hudVAO);
    if (hudVBO != 0) glDeleteBuffers(1, &hudVBO);
    if (hudShaderProgram != 0) glDeleteProgram(hudShaderProgram);
    if (fontTexture != 0) glDeleteTextures(1, &fontTexture);

    if (skyboxVAO != 0) glDeleteVertexArrays(1, &skyboxVAO);
    if (skyboxVBO != 0) glDeleteBuffers(1, &skyboxVBO);
    if (skyboxTexture != 0) glDeleteTextures(1, &skyboxTexture);

    if (nameVAO != 0) glDeleteVertexArrays(1, &nameVAO);
    if (nameVBO != 0) glDeleteBuffers(1, &nameVBO);

    GLFWwindow* window = glfwGetCurrentContext();
    if (window) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void ShootingMinigame::initializeHUD() {
    hudShaderProgram = createShader("Shaders/hud.vert", "Shaders/hud.frag");

    fontTexture = loadImageToTexture("Resources/font_texture.png");
    if (fontTexture == 0) {
        std::cerr << "Failed to load font texture for ShootingMinigame HUD!" << std::endl;
    }

    float quadVertices[] = {
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

void ShootingMinigame::initializeScene() {
    projectiles.clear();
    cats.clear();

    playerPos = glm::vec3(0.0f, 3.0f, 0.0f);  // Raised higher above ground
    playerYaw = -90.0f;
    playerPitch = 20.0f;

    spawnCats();
}

void ShootingMinigame::spawnCats() {
    // Random number of cats between 9 and 12
    int numCats = 9 + (rand() % 4);
    int realCatIndex = rand() % numCats;

    for (int i = 0; i < numCats; i++) {
        CatTarget cat;

        // Random position in a sphere above the player
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
        float dist = 5.0f + static_cast<float>(rand() % 150) / 10.0f;  // 5-20 units horizontal
        float height = CAT_SPAWN_HEIGHT_MIN + static_cast<float>(rand() % static_cast<int>((CAT_SPAWN_HEIGHT_MAX - CAT_SPAWN_HEIGHT_MIN) * 10)) / 10.0f;

        cat.position = glm::vec3(
            cos(angle) * dist,
            height,
            sin(angle) * dist
        );
        cat.alive = true;
        cat.isReal = (i == realCatIndex);
        cat.popTimer = 0.0f;
        cat.popping = false;

        cats.push_back(cat);
    }

    std::cout << "Spawned " << numCats << " cats. Real cat is #" << realCatIndex << std::endl;
}

void ShootingMinigame::update(float deltaTime) {
    if (isGameOver) {
        if (cursorLocked) {
            GLFWwindow* window = glfwGetCurrentContext();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursorLocked = false;
        }
        return;
    }

    InputManager& input = InputManager::getInstance();
    bool spaceIsPressed = input.isKeyDown(GLFW_KEY_SPACE);

    // Waiting to start
    if (waitingToStart) {
        timeElapsed += deltaTime;

        if (!spaceIsPressed) {
            spaceWasPressed = false;
        }
        if (spaceIsPressed && !spaceWasPressed) {
            waitingToStart = false;
            spaceWasPressed = true;
            timeElapsed = 0.0f;
            gameTimer = 0.0f;

            GLFWwindow* window = glfwGetCurrentContext();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cursorLocked = true;
            firstMouse = true;

            std::cout << "Shooting Game Started! Hit the real cat!" << std::endl;
        }
        return;
    }

    timeElapsed += deltaTime;

    // Name display - wait for space press to exit (but keep game interactive)
    if (showingName) {
        nameDisplayTimer += deltaTime;
        
        // Wait for space press to return to 2D
        if (!spaceIsPressed) {
            spaceWasPressed = false;
        }
        if (spaceIsPressed && !spaceWasPressed) {
            isGameOver = true;
            playerWon = true;

            GLFWwindow* window = glfwGetCurrentContext();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursorLocked = false;

            std::cout << "Returning to 2D..." << std::endl;
            return;
        }
        // Don't return - continue with normal game updates so it's not frozen
    }

    // Timer check
    gameTimer += deltaTime;
    if (gameTimer >= TIME_LIMIT) {
        isGameOver = true;
        playerWon = false;

        GLFWwindow* window = glfwGetCurrentContext();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorLocked = false;

        std::cout << "TIME'S UP! You failed to hit the real cat." << std::endl;
        return;
    }

    // Mouse look
    float mouseX = static_cast<float>(input.mouseX);
    float mouseY = static_cast<float>(input.mouseY);

    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }

    float xoffset = mouseX - lastMouseX;
    float yoffset = lastMouseY - mouseY;
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    processMouseMovement(xoffset, yoffset);

    // WASD movement (horizontal only, no jumping)
    glm::vec3 front;
    front.x = cos(glm::radians(playerYaw));
    front.y = 0.0f;
    front.z = sin(glm::radians(playerYaw));
    front = glm::normalize(front);

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 movement(0.0f);
    if (input.isKeyDown(GLFW_KEY_W)) movement += front;
    if (input.isKeyDown(GLFW_KEY_S)) movement -= front;
    if (input.isKeyDown(GLFW_KEY_A)) movement -= right;
    if (input.isKeyDown(GLFW_KEY_D)) movement += right;

    if (glm::length(movement) > 0.0f) {
        movement = glm::normalize(movement) * MOVE_SPEED * deltaTime;
        playerPos += movement;
        // Keep player above ground
        if (playerPos.y < 3.0f) playerPos.y = 3.0f;
    }

    // Slingshot charging (left mouse button)
    if (input.leftMousePressed) {
        if (!isCharging) {
            isCharging = true;
            chargeTime = 0.0f;
        }
        chargeTime += deltaTime;
        if (chargeTime > MAX_CHARGE_TIME) {
            chargeTime = MAX_CHARGE_TIME;
        }
    } else {
        if (isCharging) {
            // Fire projectile on release
            float chargeRatio = chargeTime / MAX_CHARGE_TIME;
            float speed = MAX_PROJECTILE_SPEED * chargeRatio;

            // Calculate launch direction from camera
            glm::vec3 front;
            front.x = cos(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
            front.y = sin(glm::radians(playerPitch));
            front.z = sin(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
            front = glm::normalize(front);

            Projectile proj;
            proj.position = playerPos + front * 1.5f;  // Launch from slingshot position
            proj.velocity = front * speed;
            proj.active = true;

            projectiles.push_back(proj);

            isCharging = false;
            chargeTime = 0.0f;

            std::cout << "Fired! Speed: " << speed << std::endl;
        }
    }

    // Update projectiles
    updateProjectiles(deltaTime);

    // Check hits
    checkProjectileHits();

    // Update pop animations
    for (auto& cat : cats) {
        if (cat.popping) {
            cat.popTimer += deltaTime;
            if (cat.popTimer >= POP_DURATION) {
                cat.alive = false;
                cat.popping = false;
            }
        }
    }

    // Update camera
    camera.Position = playerPos;
    camera.Yaw = playerYaw;
    camera.Pitch = playerPitch;
}

void ShootingMinigame::updateProjectiles(float deltaTime) {
    for (auto& proj : projectiles) {
        if (!proj.active) continue;

        // Apply gravity
        proj.velocity.y += PROJECTILE_GRAVITY * deltaTime;

        // Update position
        proj.position += proj.velocity * deltaTime;

        // Deactivate if fallen below ground
        if (proj.position.y < -5.0f) {
            proj.active = false;
        }

        // Deactivate if too far away
        float dist = glm::length(proj.position - playerPos);
        if (dist > 100.0f) {
            proj.active = false;
        }
    }
}

void ShootingMinigame::checkProjectileHits() {
    for (auto& proj : projectiles) {
        if (!proj.active) continue;

        for (auto& cat : cats) {
            if (!cat.alive || cat.popping) continue;

            float dist = glm::length(proj.position - cat.position);
            if (dist < CAT_HIT_RADIUS) {
                proj.active = false;

                if (cat.isReal) {
                    // Real cat hit - pop animation and trigger name display
                    cat.popping = true;
                    cat.popTimer = 0.0f;

                    // Kill all other cats
                    for (auto& otherCat : cats) {
                        if (&otherCat != &cat && otherCat.alive && !otherCat.popping) {
                            otherCat.alive = false;
                        }
                    }

                    showingName = true;
                    nameDisplayTimer = 0.0f;

                    std::cout << "HIT THE REAL CAT! Press SPACE to return to 2D." << std::endl;
                } else {
                    // Fake cat - just disappear
                    cat.alive = false;
                    std::cout << "Hit a fake cat!" << std::endl;
                }
                break;
            }
        }
    }
}

void ShootingMinigame::processMouseMovement(float xoffset, float yoffset) {
    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;

    playerYaw += xoffset;
    playerPitch += yoffset;

    if (playerPitch > 89.0f) playerPitch = 89.0f;
    if (playerPitch < -89.0f) playerPitch = -89.0f;
}

void ShootingMinigame::render() {
    // Render skybox first
    renderSkybox();

    // Re-bind shader program after skybox
    glUseProgram(shaderProgram);

    // Ensure proper OpenGL state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)screenWidth / (float)screenHeight, 0.1f, 200.0f);

    // Build view matrix from yaw/pitch
    glm::vec3 front;
    front.x = cos(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front.y = sin(glm::radians(playerPitch));
    front.z = sin(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front = glm::normalize(front);

    glm::mat4 view = glm::lookAt(playerPos, playerPos + front, glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(playerPos));

    // Directional light (bright daylight)
    glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f));
    glm::vec3 lightColor = glm::vec3(1.0f, 0.95f, 0.9f);
    glm::vec3 ambientLight = glm::vec3(0.4f, 0.4f, 0.45f);

    glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "ambientLight"), 1, glm::value_ptr(ambientLight));

    // No fog
    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 1);
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), 0.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "fogColor"), 0.0f, 0.0f, 0.0f);

    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);

    // Render world
    renderGround();
    renderCats();
    renderProjectiles();
    renderSlingshot();

    // Name billboard (always rendered, but invisible until real cat is hit)
    renderNameBillboard();

    // HUD on top (crosshair, timer, charge bar)
    renderCrosshair();
    renderHUD();
}

void ShootingMinigame::renderGround() {
    GLboolean wasCullingEnabled = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
    model = glm::scale(model, glm::vec3(100.0f, 0.5f, 100.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);

    groundModel.render();

    if (wasCullingEnabled) {
        glEnable(GL_CULL_FACE);
    }
}

void ShootingMinigame::renderCats() {
    for (const auto& cat : cats) {
        if (!cat.alive && !cat.popping) continue;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cat.position);

        // Pop animation: scale up then fade
        float scale = CAT_SCALE;
        if (cat.popping) {
            float progress = cat.popTimer / POP_DURATION;
            scale = CAT_SCALE * (1.0f + progress * 3.0f);  // Expand during pop
        }

        // Make cats face the player
        glm::vec3 toPlayer = glm::normalize(playerPos - cat.position);
        float yaw = atan2(toPlayer.x, toPlayer.z);
        model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::scale(model, glm::vec3(scale));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // Pop effect: change color during pop animation
        if (cat.popping) {
            float progress = cat.popTimer / POP_DURATION;
            float flash = 1.0f + progress * 4.0f;  // Bright flash
            glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), flash, flash * 0.5f, 0.0f);
        } else {
            glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);
        }

        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);

        catModel.render();
    }

    // Reset color
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);
}

void ShootingMinigame::renderProjectiles() {
    for (const auto& proj : projectiles) {
        if (!proj.active) continue;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, proj.position);
        model = glm::scale(model, glm::vec3(0.15f));  // Small pumpkin

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);

        pumpkinModel.render();
    }
}

void ShootingMinigame::renderSlingshot() {
    // Calculate camera front direction
    glm::vec3 front;
    front.x = cos(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front.y = sin(glm::radians(playerPitch));
    front.z = sin(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front = glm::normalize(front);

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    // Position slingshot in front of camera, offset to bottom-right (lowered Y)
    glm::vec3 slingshotPos = playerPos + front * 0.8f + right * 0.3f - up * 0.5f;

    // Pull back during charge
    float pullBack = 0.0f;
    if (isCharging) {
        float chargeRatio = chargeTime / MAX_CHARGE_TIME;
        pullBack = chargeRatio * 0.15f;
        slingshotPos -= front * pullBack;
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, slingshotPos);

    // Rotate slingshot to face same direction as camera
    model = glm::rotate(model, glm::radians(-playerYaw - 210.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(playerPitch), glm::vec3(1.0f, 0.0f, 0.0f));

    // Scale slingshot (adjust as needed for model size)
    float slingshotScale = 0.04f;

    // Stretch effect during charging
    if (isCharging) {
        float chargeRatio = chargeTime / MAX_CHARGE_TIME;
        // Stretch the Z axis (depth) to simulate band pulling
        model = glm::scale(model, glm::vec3(slingshotScale, slingshotScale, slingshotScale * (1.0f + chargeRatio * 0.5f)));
    } else {
        model = glm::scale(model, glm::vec3(slingshotScale));
    }

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);

    slingshotModel.render();
}

void ShootingMinigame::renderCrosshair() {
    glDisable(GL_DEPTH_TEST);

    glUseProgram(shaderProgram);

    glm::mat4 orthoProj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 identityView = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(orthoProj));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(identityView));
    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 0);

    // Horizontal crosshair line
    glm::mat4 hLine = glm::mat4(1.0f);
    hLine = glm::translate(hLine, glm::vec3(0.0f, 0.0f, 0.0f));
    hLine = glm::scale(hLine, glm::vec3(0.04f, 0.003f, 0.1f));
    renderCube(hLine, glm::vec3(1.0f, 1.0f, 1.0f));

    // Vertical crosshair line
    glm::mat4 vLine = glm::mat4(1.0f);
    vLine = glm::translate(vLine, glm::vec3(0.0f, 0.0f, 0.0f));
    vLine = glm::scale(vLine, glm::vec3(0.003f, 0.04f, 0.1f));
    renderCube(vLine, glm::vec3(1.0f, 1.0f, 1.0f));

    // Center dot
    glm::mat4 dot = glm::mat4(1.0f);
    dot = glm::translate(dot, glm::vec3(0.0f, 0.0f, 0.1f));
    dot = glm::scale(dot, glm::vec3(0.006f, 0.006f, 0.1f));
    renderCube(dot, glm::vec3(1.0f, 0.2f, 0.2f));

    glEnable(GL_DEPTH_TEST);
}

void ShootingMinigame::renderHUD() {
    if (waitingToStart) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    glm::mat4 orthoProj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 identityView = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(orthoProj));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(identityView));
    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 0);

    // Timer bar (top of screen)
    float timeRemaining = TIME_LIMIT - gameTimer;
    if (timeRemaining < 0.0f) timeRemaining = 0.0f;

    float barWidth = 0.8f;
    float barHeight = 0.04f;
    float barY = 0.9f;

    // Background bar
    glm::mat4 bgBar = glm::mat4(1.0f);
    bgBar = glm::translate(bgBar, glm::vec3(0.0f, barY, 0.0f));
    bgBar = glm::scale(bgBar, glm::vec3(barWidth, barHeight, 0.1f));
    renderCube(bgBar, glm::vec3(0.2f, 0.2f, 0.2f));

    // Progress bar
    float progress = timeRemaining / TIME_LIMIT;
    float progressWidth = barWidth * progress;
    float offsetX = -(barWidth - progressWidth) / 2.0f;
    glm::mat4 progressBar = glm::mat4(1.0f);
    progressBar = glm::translate(progressBar, glm::vec3(offsetX, barY, 0.1f));
    progressBar = glm::scale(progressBar, glm::vec3(progressWidth, barHeight * 0.8f, 0.1f));

    glm::vec3 timerColor;
    if (progress > 0.5f) timerColor = glm::vec3(0.2f, 0.8f, 0.2f);
    else if (progress > 0.25f) timerColor = glm::vec3(0.9f, 0.7f, 0.1f);
    else timerColor = glm::vec3(0.9f, 0.2f, 0.2f);
    renderCube(progressBar, timerColor);

    // Charge bar (bottom of screen, only when charging)
    if (isCharging) {
        float chargeRatio = chargeTime / MAX_CHARGE_TIME;

        // Charge background
        glm::mat4 chargeBg = glm::mat4(1.0f);
        chargeBg = glm::translate(chargeBg, glm::vec3(0.0f, -0.85f, 0.0f));
        chargeBg = glm::scale(chargeBg, glm::vec3(0.4f, 0.03f, 0.1f));
        renderCube(chargeBg, glm::vec3(0.15f, 0.15f, 0.15f));

        // Charge fill
        float chargeWidth = 0.4f * chargeRatio;
        float chargeOffsetX = -(0.4f - chargeWidth) / 2.0f;
        glm::mat4 chargeFill = glm::mat4(1.0f);
        chargeFill = glm::translate(chargeFill, glm::vec3(chargeOffsetX, -0.85f, 0.1f));
        chargeFill = glm::scale(chargeFill, glm::vec3(chargeWidth, 0.025f, 0.1f));

        // Color from yellow to red as charge increases
        glm::vec3 chargeColor = glm::mix(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), chargeRatio);
        renderCube(chargeFill, chargeColor);
    }

    glEnable(GL_DEPTH_TEST);
}

void ShootingMinigame::renderLetter(char letter, glm::vec3 position, float size, glm::vec3 color) {
    // 5x7 grid pattern for each letter (1 = cube, 0 = empty)
    std::vector<std::vector<int>> pattern;
    
    switch(letter) {
        case 'M':
            pattern = {
                {1,0,0,0,1},
                {1,1,0,1,1},
                {1,0,1,0,1},
                {1,0,0,0,1},
                {1,0,0,0,1},
                {1,0,0,0,1},
                {1,0,0,0,1}
            }; break;
        case 'I':
            pattern = {
                {1,1,1},
                {0,1,0},
                {0,1,0},
                {0,1,0},
                {0,1,0},
                {0,1,0},
                {1,1,1}
            }; break;
        case 'L':
            pattern = {
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,1,1,1}
            }; break;
        case 'A':
            pattern = {
                {0,1,1,0},
                {1,0,0,1},
                {1,0,0,1},
                {1,1,1,1},
                {1,0,0,1},
                {1,0,0,1},
                {1,0,0,1}
            }; break;
        case 'N':
            pattern = {
                {1,0,0,1},
                {1,1,0,1},
                {1,0,1,1},
                {1,0,0,1},
                {1,0,0,1},
                {1,0,0,1},
                {1,0,0,1}
            }; break;
        case 'S':
            pattern = {
                {0,1,1,1},
                {1,0,0,0},
                {1,0,0,0},
                {0,1,1,0},
                {0,0,0,1},
                {0,0,0,1},
                {1,1,1,0}
            }; break;
        case 'P':
            pattern = {
                {1,1,1,0},
                {1,0,0,1},
                {1,0,0,1},
                {1,1,1,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0}
            }; break;
        case 'O':
            pattern = {
                {0,1,1,0},
                {1,0,0,1},
                {1,0,0,1},
                {1,0,0,1},
                {1,0,0,1},
                {1,0,0,1},
                {0,1,1,0}
            }; break;
        case 'J':
            pattern = {
                {0,0,1},
                {0,0,1},
                {0,0,1},
                {0,0,1},
                {1,0,1},
                {1,0,1},
                {0,1,0}
            }; break;
        case 'E':
            pattern = {
                {1,1,1,1},
                {1,0,0,0},
                {1,0,0,0},
                {1,1,1,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,1,1,1}
            }; break;
        case 'V':
            pattern = {
                {1,0,0,1},
                {1,0,0,1},
                {1,0,0,1},
                {1,0,0,1},
                {0,1,1,0},
                {0,1,1,0},
                {0,0,0,0}
            }; break;
        case 'C':
            pattern = {
                {0,1,1,1},
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0},
                {0,1,1,1}
            }; break;
        case 'R':
            pattern = {
                {1,1,1,0},
                {1,0,0,1},
                {1,0,0,1},
                {1,1,1,0},
                {1,0,1,0},
                {1,0,0,1},
                {1,0,0,1}
            }; break;
        case '1':
            pattern = {
                {0,1,0},
                {1,1,0},
                {0,1,0},
                {0,1,0},
                {0,1,0},
                {0,1,0},
                {1,1,1}
            }; break;
        case '4':
            pattern = {
                {1,0,1},
                {1,0,1},
                {1,0,1},
                {1,1,1},
                {0,0,1},
                {0,0,1},
                {0,0,1}
            }; break;
        case '7':
            pattern = {
                {1,1,1,1},
                {0,0,0,1},
                {0,0,1,0},
                {0,1,0,0},
                {0,1,0,0},
                {0,1,0,0},
                {0,1,0,0}
            }; break;
        case '2':
            pattern = {
                {1,1,1},
                {0,0,1},
                {0,0,1},
                {1,1,1},
                {1,0,0},
                {1,0,0},
                {1,1,1}
            }; break;
        case '0':
            pattern = {
                {0,1,1,0},
                {1,0,0,1},
                {1,0,1,1},
                {1,0,1,1},
                {1,1,0,1},
                {1,0,0,1},
                {0,1,1,0}
            }; break;
        case ' ':
            pattern = {
                {0,0},
                {0,0},
                {0,0},
                {0,0},
                {0,0},
                {0,0},
                {0,0}
            }; break;
        default:
            pattern = {
                {1,1,1},
                {1,0,1},
                {1,0,1},
                {1,0,1},
                {1,0,1},
                {1,0,1},
                {1,1,1}
            }; break;
    }
    
    // Render each cube in the pattern
    for (int row = 0; row < pattern.size(); row++) {
        for (int col = 0; col < pattern[row].size(); col++) {
            if (pattern[row][col] == 1) {
                glm::vec3 cubePos = position + glm::vec3(col * size, -(row * size), 0.0f);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePos);
                model = glm::scale(model, glm::vec3(size * 0.9f)); // 0.9 to have small gaps
                renderCube(model, color);
            }
        }
    }
}

void ShootingMinigame::renderText(const std::string& text, glm::vec3 startPos, float size, float spacing, glm::vec3 color) {
    glm::vec3 currentPos = startPos;
    
    for (char c : text) {
        if (c == ' ') {
            currentPos.x += spacing * 1.2f; // Reduced space width for tighter spacing
        } else {
            renderLetter(c, currentPos, size, color);
            currentPos.x += spacing; // Move to next letter position
        }
    }
}

void ShootingMinigame::renderNameBillboard() {
    // Calculate visibility based on whether real cat was hit
    float alpha = 0.0f;
    if (showingName) {
        alpha = glm::min(nameDisplayTimer / 0.5f, 1.0f);
    }
    
    if (alpha <= 0.0f) return; // Don't render if invisible
    
    // Position high in the sky, centered
    glm::vec3 skyTextPos = glm::vec3(-13.0f, 25.0f, -10.0f);
    
    // Line 1: "MILAN SPASOJEVIC"
    renderText("MILAN SPASOJEVIC", skyTextPos, 0.3f, 1.8f, glm::vec3(alpha * 0.5f, alpha * 0.5f, alpha * 0.5f));
    
    // Line 2: "RA 147 2022"
    glm::vec3 line2Pos = skyTextPos + glm::vec3(3.5f, -2.8f, 0.0f);
    renderText("RA 147 2022", line2Pos, 0.25f, 1.5f, glm::vec3(alpha * 0.45f, alpha * 0.45f, alpha * 0.5f));
    
    // "PRESS SPACE" - just a guide text
    if (showingName) {
        float pulse = 0.5f + 0.5f * sin(nameDisplayTimer * 3.0f);
        glm::vec3 promptPos = skyTextPos + glm::vec3(4.0f, -5.2f, 0.0f);
        renderText("PRESS SPACE", promptPos, 0.2f, 1.2f, glm::vec3(pulse * alpha * 0.5f, pulse * alpha * 0.5f, alpha * 0.5f));
    }
}

void ShootingMinigame::renderSkybox() {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    glUseProgram(shaderProgram);

    glm::vec3 front;
    front.x = cos(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front.y = sin(glm::radians(playerPitch));
    front.z = sin(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front = glm::normalize(front);

    glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(glm::vec3(0.0f), front, glm::vec3(0.0f, 1.0f, 0.0f))));
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)screenWidth / (float)screenHeight, 0.1f, 200.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glm::vec3 origin(0.0f);
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(origin));

    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 1);
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), 0.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "fogColor"), 0.0f, 0.0f, 0.0f);

    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);

    glBindVertexArray(skyboxVAO);
    glVertexAttrib3f(2, 0.0f, 1.0f, 0.0f);
    glVertexAttrib3f(3, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
}

void ShootingMinigame::renderCube(glm::mat4 model, glm::vec3 color) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, glm::value_ptr(color));
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0);

    glBindVertexArray(VAO);
    glDisableVertexAttribArray(3);
    glVertexAttrib3f(3, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

bool ShootingMinigame::checkWinCondition() {
    return playerWon;
}

bool ShootingMinigame::isFinished() {
    return isGameOver;
}
