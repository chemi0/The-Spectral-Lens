#include "../Header/RunnerMinigame.h"
#include "../Header/InputManager.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

const float LANE_WIDTH = 2.5f;
const float PLAYER_SPEED = 15.0f;
const float MAX_SURVIVAL_TIME = 30.0f; // Player needs to survive for 30 seconds to win

// Scale factors for player and obstacles
const float PLAYER_SCALE = 0.003f;      // Smaller player (was 0.005f)
const float OBSTACLE_SCALE = 0.4f;      // Reasonable scale for the pumpkin obstacle model

// Fog settings
const float FOG_DENSITY = 0.08f;        // Higher fog density for difficulty
const glm::vec3 FOG_COLOR = glm::vec3(0.1f, 0.12f, 0.1f);

RunnerMinigame::RunnerMinigame(int width, int height)
: screenWidth(width), screenHeight(height),
isGameOver(false), playerWon(false), timeElapsed(0.0f),
currentLane(1), playerX(0.0f), targetX(0.0f),
spawnTimer(0.0f), gameSpeed(20.0f), survivalTimer(0.0f),  // Faster game speed (was 15.0f)
verticalVelocity(0.0f), isJumping(false), playerScaleY(1.0f), targetScaleY(1.0f),
camera(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.f, -20.0f), // Adjusted camera for smaller player
hudShaderProgram(0), hudVAO(0), hudVBO(0), fontTexture(0), groundOffset(0.0f),
waitingToStart(true), spaceWasPressed(true)
{
	// Load Shaders
	shaderProgram = createShader("Shaders/basic_3d.vert", "Shaders/basic_3d.frag");

	// Cube Mesh (For floor and HUD elements)
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

    playerModel.normalMapTex = playerModel.loadTexture("Resources/RunnerCharacter/cat_Normal.png");
    if (playerModel.normalMapTex == 0) std::cerr << "Failed to load normal texture!" << std::endl;

    // Load obstacle model (pumpkin)
    if (!obstacleModel.loadModel("Resources/RunnerObstacle/obstacle.obj")) {
        std::cerr << "Failed to load obstacle model!" << std::endl;
    }

    obstacleModel.diffuseTex = obstacleModel.loadTexture("Resources/RunnerObstacle/obstacleBase.png");
    if (obstacleModel.diffuseTex == 0) std::cerr << "Failed to load obstacle diffuse texture!" << std::endl;

    obstacleModel.aoTex = obstacleModel.loadTexture("Resources/RunnerObstacle/obstacleAO.png");
    if (obstacleModel.aoTex == 0) std::cerr << "Failed to load obstacle AO texture!" << std::endl;

    obstacleModel.normalMapTex = obstacleModel.loadTexture("Resources/RunnerObstacle/obstacleNormal.png");
    if (obstacleModel.normalMapTex == 0) std::cerr << "Failed to load obstacle normal texture!" << std::endl;

    obstacleModel.roughnessTex = obstacleModel.loadTexture("Resources/RunnerObstacle/obstacleRoughness.png");
    if (obstacleModel.roughnessTex == 0) std::cerr << "Failed to load obstacle roughness texture!" << std::endl;

    // Load ground platform model (textured rock wall)
    if (!groundModel.loadModel("Resources/PlatformTexture/rock_wall_16_1k.obj")) {
        std::cerr << "Failed to load ground model!" << std::endl;
    }

    groundModel.diffuseTex = groundModel.loadTexture("Resources/PlatformTexture/diffWall.png");
    if (groundModel.diffuseTex == 0) std::cerr << "Failed to load ground diffuse texture!" << std::endl;

    groundModel.normalMapTex = groundModel.loadTexture("Resources/PlatformTexture/normalWall.png");
    if (groundModel.normalMapTex == 0) std::cerr << "Failed to load ground normal texture!" << std::endl;

    groundModel.aoTex = groundModel.loadTexture("Resources/PlatformTexture/armWall.png");
    if (groundModel.aoTex == 0) std::cerr << "Failed to load ground AO texture!" << std::endl;

    // Load skybox cubemap
    skyboxTexture = loadImageToTexture("Resources/SkyboxCubemaps/RunnerCubemap.png");
    if (skyboxTexture == 0) {
        std::cerr << "Failed to load skybox texture!" << std::endl;
    } else {
        std::cout << "Skybox texture loaded successfully with ID: " << skyboxTexture << std::endl;
    }

    // Create skybox cube VAO with position and UV coordinates
    // Fixed UV mapping to sample from the sunset band in center of image
    float skyboxVertices[] = {
        // positions                // UV coords (sunset is in center horizontal band)
        -1.0f,  1.0f, -1.0f,        0.25f, 0.40f,
        -1.0f, -1.0f, -1.0f,        0.25f, 0.60f,
         1.0f, -1.0f, -1.0f,        0.50f, 0.60f,
         1.0f, -1.0f, -1.0f,        0.50f, 0.60f,
         1.0f,  1.0f, -1.0f,        0.50f, 0.40f,
        -1.0f,  1.0f, -1.0f,        0.25f, 0.40f,

        -1.0f, -1.0f,  1.0f,        0.00f, 0.60f,
        -1.0f, -1.0f, -1.0f,        0.25f, 0.60f,
        -1.0f,  1.0f, -1.0f,        0.25f, 0.40f,
        -1.0f,  1.0f, -1.0f,        0.25f, 0.40f,
        -1.0f,  1.0f,  1.0f,        0.00f, 0.40f,
        -1.0f, -1.0f,  1.0f,        0.00f, 0.60f,

         1.0f, -1.0f, -1.0f,        0.50f, 0.60f,
         1.0f, -1.0f,  1.0f,        0.75f, 0.60f,
         1.0f,  1.0f,  1.0f,        0.75f, 0.40f,
         1.0f,  1.0f,  1.0f,        0.75f, 0.40f,
         1.0f,  1.0f, -1.0f,        0.50f, 0.40f,
         1.0f, -1.0f, -1.0f,        0.50f, 0.60f,

        -1.0f, -1.0f,  1.0f,        0.00f, 0.60f,
        -1.0f,  1.0f,  1.0f,        0.00f, 0.40f,
         1.0f,  1.0f,  1.0f,        0.75f, 0.40f,
         1.0f,  1.0f,  1.0f,        0.75f, 0.40f,
         1.0f, -1.0f,  1.0f,        0.75f, 0.60f,
        -1.0f, -1.0f,  1.0f,        0.00f, 0.60f,

        -1.0f,  1.0f, -1.0f,        0.25f, 0.30f,
         1.0f,  1.0f, -1.0f,        0.50f, 0.30f,
         1.0f,  1.0f,  1.0f,        0.75f, 0.30f,
         1.0f,  1.0f,  1.0f,        0.75f, 0.30f,
        -1.0f,  1.0f,  1.0f,        0.00f, 0.30f,
        -1.0f,  1.0f, -1.0f,        0.25f, 0.30f,

        -1.0f, -1.0f, -1.0f,        0.25f, 0.70f,
        -1.0f, -1.0f,  1.0f,        0.00f, 0.70f,
         1.0f, -1.0f, -1.0f,        0.50f, 0.70f,
         1.0f, -1.0f, -1.0f,        0.50f, 0.70f,
        -1.0f, -1.0f,  1.0f,        0.00f, 0.70f,
         1.0f, -1.0f,  1.0f,        0.75f, 0.70f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // UV coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

    // Initialize HUD
    initializeHUD();

    std::cout << "Runner Minigame Initialized!" << std::endl;
    std::cout << "Survive for " << MAX_SURVIVAL_TIME << " seconds!" << std::endl;
    std::cout << "Controls: A/D or Left/Right = Switch lanes, SPACE = Jump, CTRL = Duck" << std::endl;
}

RunnerMinigame::~RunnerMinigame() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    // Clean up HUD resources
    if (hudVAO != 0) glDeleteVertexArrays(1, &hudVAO);
    if (hudVBO != 0) glDeleteBuffers(1, &hudVBO);
    if (hudShaderProgram != 0) glDeleteProgram(hudShaderProgram);
    if (fontTexture != 0) glDeleteTextures(1, &fontTexture);
    
    // Clean up skybox
    if (skyboxVAO != 0) glDeleteVertexArrays(1, &skyboxVAO);
    if (skyboxVBO != 0) glDeleteBuffers(1, &skyboxVBO);
    if (skyboxTexture != 0) glDeleteTextures(1, &skyboxTexture);
}

void RunnerMinigame::initializeHUD() {
    // Load HUD shader
    hudShaderProgram = createShader("Shaders/hud.vert", "Shaders/hud.frag");
    
    // Load font texture
    fontTexture = loadImageToTexture("Resources/font_texture.png");
    if (fontTexture == 0) {
        std::cerr << "Failed to load font texture for Runner HUD!" << std::endl;
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

void RunnerMinigame::update(float deltaTime) {
if (isGameOver) return;

InputManager& input = InputManager::getInstance();
bool spaceIsPressed = input.isKeyDown(GLFW_KEY_SPACE);

// Handle "Press Space to Start" screen
if (waitingToStart) {
timeElapsed += deltaTime;

if (!spaceIsPressed) {
spaceWasPressed = false;
}
if (spaceIsPressed && !spaceWasPressed) {
waitingToStart = false;
spaceWasPressed = true;
timeElapsed = 0.0f;
survivalTimer = 0.0f;

std::cout << "Game Started! Survive for " << MAX_SURVIVAL_TIME << " seconds!" << std::endl;
}
return;  // Don't update game logic while waiting
}

    survivalTimer += deltaTime;
    timeElapsed += deltaTime;

    bool isRightDown = input.isKeyDown(GLFW_KEY_D) || input.isKeyDown(GLFW_KEY_RIGHT);
    if (isRightDown && !wasRightPressed) {
        if (currentLane < 2) currentLane++;
    }
    wasRightPressed = isRightDown;

	bool isLeftDown = input.isKeyDown(GLFW_KEY_A) || input.isKeyDown(GLFW_KEY_LEFT);
    if (isLeftDown && !wasLeftPressed) {
        if (currentLane > 0) currentLane--;
	}
	wasLeftPressed = isLeftDown;

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
        targetScaleY = 0.5f;
    } else {
        targetScaleY = 1.0f;
	}

	// Smooth Ducking Transition
    if (playerScaleY > targetScaleY) {
        playerScaleY -= DUCK_SPEED * deltaTime;
		if (playerScaleY < targetScaleY) playerScaleY = targetScaleY;
    }
    else if (playerScaleY < targetScaleY) {
		playerScaleY += DUCK_SPEED * deltaTime;
		if (playerScaleY > targetScaleY) playerScaleY = targetScaleY;
    }

    // Jumping (Press Space)
	if (input.isKeyDown(GLFW_KEY_SPACE) && !isJumping && playerY <= 0.05f) {
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

    // Obstacle Spawning - More aggressive spawning
    spawnTimer -= deltaTime;
    if (spawnTimer <= 0.0f) {
        spawnObstacle();
        
		// Spawn faster over time - more aggressive curve
        spawnTimer = 1.0f - (survivalTimer * 0.025f);  // Faster base spawn (was 1.5f)
        if (spawnTimer < 0.3f) spawnTimer = 0.3f;      // Lower minimum (was 0.4f)
    }

    // Obstacle Logic with scaled hitboxes
    float playerZ = 0.0f;
    float hitboxWidth = 0.4f * OBSTACLE_SCALE;  // Smaller hitbox

    for (auto& obs : obstacles) {
		obs.position.z += gameSpeed * deltaTime;

        // Collision Checks with adjusted hitboxes
        if (obs.position.z > -0.3f && obs.position.z < 0.3f) {

            if (fabs(playerX - obs.position.x) < (0.5f + hitboxWidth)) {

                bool hit = false;
                
                if (obs.type == GROUND) {
                    if (playerY < 0.4f * OBSTACLE_SCALE) hit = true;
                }
                else if (obs.type == FLYING_LOW) {
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
    
	// Remove obstacles that have passed the player
    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), 
		[](Obstacle& o) { return o.position.z > 5.0f; }), obstacles.end());

    // Update ground scroll offset (creates illusion of movement)
    groundOffset += gameSpeed * deltaTime;
    // Wrap offset to prevent floating-point precision issues
    if (groundOffset > 100.0f) {
        groundOffset -= 100.0f;
    }

    // Win Condition
    if (survivalTimer >= MAX_SURVIVAL_TIME) {
        isGameOver = true;
        playerWon = true;
		std::cout << "VICTORY! You survived the runner minigame." << std::endl;
    }
}

void RunnerMinigame::spawnObstacle() {
    int lane = rand() % 3;
    float xPos = (lane - 1) * LANE_WIDTH;

    Obstacle obs;
    obs.active = true;

    // 40% chance for a flying obstacle
    if (rand() % 100 < 40) {
        obs.type = FLYING_LOW;
        obs.position = glm::vec3(xPos, 1.2f, -50.0f);  // Flying at head height
    }
    else {
        obs.type = GROUND;
        obs.position = glm::vec3(xPos, 0.3f, -50.0f);  // Ground level
    }

	obstacles.push_back(obs);
    
    // Chance to spawn a second obstacle in a different lane for extra difficulty
    if (rand() % 100 < 30 && survivalTimer > 5.0f) {  // 30% chance after 5 seconds
        int secondLane = rand() % 3;
        if (secondLane != lane) {
            Obstacle obs2;
            obs2.active = true;
            obs2.type = (rand() % 100 < 40) ? FLYING_LOW : GROUND;
            obs2.position = glm::vec3((secondLane - 1) * LANE_WIDTH, 
                                       (obs2.type == FLYING_LOW) ? 1.2f : 0.3f, 
                                       -50.0f);
            obstacles.push_back(obs2);
        }
    }
}

void RunnerMinigame::render() {
// Render skybox FIRST (behind everything)
renderSkybox();

glUseProgram(shaderProgram);

    // Set fog uniforms
    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 1);
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), FOG_DENSITY);
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(FOG_COLOR));

    // Matrices
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(screenWidth) / float(screenHeight), 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
	unsigned int camPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(camPosLoc, 1, glm::value_ptr(camera.Position));

    // Set up directional light (sunset/sunrise lighting to match skybox)
    glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -0.8f, -0.3f));
    glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.7f);  // Warm orange/yellow light
    glm::vec3 ambientLight = glm::vec3(0.4f, 0.35f, 0.3f);  // Warm ambient to match skybox
    
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "ambientLight"), 1, glm::value_ptr(ambientLight));

    // Calculate Player Matrix with smaller scale
    glm::mat4 playerMatrix = glm::mat4(1.0f);
    playerMatrix = glm::translate(playerMatrix, glm::vec3(playerX, playerY, 0.0f));
    playerMatrix = glm::scale(playerMatrix, glm::vec3(PLAYER_SCALE));  // Smaller player

    // Ducking animation
    float centerOffset = (1.0f - playerScaleY) * -0.5f;
    playerMatrix = glm::translate(playerMatrix, glm::vec3(0.0f, centerOffset * 20.0f, 0.0f));
    playerMatrix = glm::scale(playerMatrix, glm::vec3(1.0f, playerScaleY, 1.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(playerMatrix));

    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);

    playerModel.render();

    // Render Obstacles (pumpkin models)
    for (const auto& obs : obstacles) {
        renderObstacle(obs);
    }

    // Floor (Textured Rock Wall Platform) - with scrolling motion
    // Render multiple copies to create seamless infinite runway
    const float PLATFORM_DEPTH = 60.0f;
    
    for (int i = -1; i <= 1; i++) {  // Render 3 copies: behind, current, ahead
        glm::mat4 floorModel = glm::mat4(1.0f);
        floorModel = glm::translate(floorModel, glm::vec3(0.0f, -0.5f, -25.0f + groundOffset + (i * PLATFORM_DEPTH)));
        floorModel = glm::scale(floorModel, glm::vec3(LANE_WIDTH * 3 + 2.0f, 0.5f, PLATFORM_DEPTH));
        
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(floorModel));
        
        // Enable texturing for ground
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);
        
        groundModel.render();
    }

    // Render timer HUD
    renderTimerHUD();

    // Render "Press Space to Start" indicator if waiting
    if (waitingToStart) {
        renderStartPrompt();
    }
}

void RunnerMinigame::renderObstacle(const Obstacle& obs) {
    glm::mat4 obsMatrix = glm::mat4(1.0f);
    
    // Position the obstacle
    obsMatrix = glm::translate(obsMatrix, obs.position);
    
    // Rotate to face the player (180 degrees around Y axis so it faces towards +Z)
    obsMatrix = glm::rotate(obsMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Scale the obstacle model
    obsMatrix = glm::scale(obsMatrix, glm::vec3(OBSTACLE_SCALE));
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(obsMatrix));
    
    // Set texture uniforms for obstacle
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);
    
    obstacleModel.render();
}

void RunnerMinigame::renderTimerHUD() {
    // Calculate remaining time
    float timeRemaining = MAX_SURVIVAL_TIME - survivalTimer;
    if (timeRemaining < 0.0f) timeRemaining = 0.0f;

    // Disable depth test for HUD
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    // Render timer
    
    // Background bar (dark)
    glm::mat4 barBgModel = glm::mat4(1.0f);
    float barWidth = 0.8f;
    float barHeight = 0.05f;
    float barY = 0.9f;
    
    // Set orthographic projection for HUD
    glm::mat4 orthoProj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 identityView = glm::mat4(1.0f);
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(orthoProj));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(identityView));
    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 0);  // Disable fog for HUD
    
    // Background bar
    glm::mat4 bgBar = glm::mat4(1.0f);
    bgBar = glm::translate(bgBar, glm::vec3(0.0f, barY, 0.0f));
    bgBar = glm::scale(bgBar, glm::vec3(barWidth, barHeight, 0.1f));
    renderCube(bgBar, glm::vec3(0.2f, 0.2f, 0.2f));
    
    // Progress bar (shows time remaining)
    float progress = timeRemaining / MAX_SURVIVAL_TIME;
    glm::mat4 progressBar = glm::mat4(1.0f);
    float progressWidth = barWidth * progress;
    float offsetX = -(barWidth - progressWidth) / 2.0f;
    progressBar = glm::translate(progressBar, glm::vec3(offsetX, barY, 0.1f));
    progressBar = glm::scale(progressBar, glm::vec3(progressWidth, barHeight * 0.8f, 0.1f));
    
    // Color changes as time runs out (green -> yellow -> red)
    glm::vec3 timerColor;
    if (progress > 0.5f) {
        timerColor = glm::vec3(0.2f, 0.8f, 0.2f);  // Green
    } else if (progress > 0.25f) {
        timerColor = glm::vec3(0.9f, 0.7f, 0.1f);  // Yellow
    } else {
        timerColor = glm::vec3(0.9f, 0.2f, 0.2f);  // Red
    }
    renderCube(progressBar, timerColor);
    
    // Render seconds remaining as blocks (each block = 5 seconds)
    int secondsRemaining = static_cast<int>(timeRemaining);
    float blockStartX = -0.35f;
    for (int i = 0; i < 6; i++) {  // 6 blocks for 30 seconds
        glm::mat4 blockModel = glm::mat4(1.0f);
        blockModel = glm::translate(blockModel, glm::vec3(blockStartX + i * 0.12f, barY - 0.1f, 0.0f));
        blockModel = glm::scale(blockModel, glm::vec3(0.08f, 0.04f, 0.1f));
        
        bool blockActive = (secondsRemaining > i * 5);
        glm::vec3 blockColor = blockActive ? timerColor : glm::vec3(0.15f, 0.15f, 0.15f);
        renderCube(blockModel, blockColor);
    }

    // Re-enable depth test
    glEnable(GL_DEPTH_TEST);
}

void RunnerMinigame::renderCube(glm::mat4 model, glm::vec3 color) {
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    unsigned int useTexLoc = glGetUniformLocation(shaderProgram, "useTexture");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));
    glUniform1i(useTexLoc, 0);

    glBindVertexArray(VAO);
    glDisableVertexAttribArray(3);
     glVertexAttrib3f(3, 1.0f, 0.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 0, 36);
     glBindVertexArray(0);
}

void RunnerMinigame::renderStartPrompt() {
    // Render a pulsing indicator above the player to signal "Press Space"
    glUseProgram(shaderProgram);
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(screenWidth) / float(screenHeight), 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    // Pulsing effect
    float pulse = 0.5f + 0.5f * sin(timeElapsed * 4.0f);
    float scale = 0.3f + 0.1f * pulse;
    
    // Floating indicator above player
    glm::mat4 indicatorModel = glm::mat4(1.0f);
    indicatorModel = glm::translate(indicatorModel, glm::vec3(playerX, playerY + 2.5f + 0.3f * sin(timeElapsed * 2.0f), 0.0f));
    indicatorModel = glm::rotate(indicatorModel, timeElapsed * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    indicatorModel = glm::scale(indicatorModel, glm::vec3(scale));
    
    // Pulsing cyan color for visibility
    glm::vec3 indicatorColor = glm::vec3(0.0f, pulse, 1.0f);
    renderCube(indicatorModel, indicatorColor);
    
    // Render a second indicator - an arrow pointing down
    glm::mat4 arrowModel = glm::mat4(1.0f);
    arrowModel = glm::translate(arrowModel, glm::vec3(playerX, playerY + 1.8f + 0.1f * sin(timeElapsed * 3.0f), 0.0f));
    arrowModel = glm::scale(arrowModel, glm::vec3(0.15f, 0.4f, 0.15f));
    renderCube(arrowModel, glm::vec3(1.0f, 1.0f, 1.0f)); // White arrow
}

void RunnerMinigame::renderSkybox() {
    // Disable depth writing so skybox is always behind
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    
    // Disable face culling so we can see inside the cube
    glDisable(GL_CULL_FACE);

    glUseProgram(shaderProgram);

    // Remove translation from view matrix (keep only rotation)
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(screenWidth) / float(screenHeight), 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    // Keep skybox small enough to avoid fog issues
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // Set camera position to origin (since we removed translation from view)
    glm::vec3 origin(0.0f);
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(origin));

    // Use fog with zero density to completely disable fog effect
    // (the else branch in shader still applies default fog, so we must use useFog=true with density=0)
    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 1);
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), 0.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "fogColor"), 0.0f, 0.0f, 0.0f);
    
    // Use texture mode with white tint
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);

    // Bind skybox texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);

    // Render skybox cube using skybox VAO
    // Provide default normal and tangent attributes since skybox VAO doesn't have them
    glBindVertexArray(skyboxVAO);
    glVertexAttrib3f(2, 0.0f, 1.0f, 0.0f);  // Default normal (up)
    glVertexAttrib3f(3, 1.0f, 0.0f, 0.0f);  // Default tangent
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Restore depth and culling settings
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
}

bool RunnerMinigame::checkWinCondition() {
    return playerWon;
}

bool RunnerMinigame::isFinished() {
    return isGameOver;
}
