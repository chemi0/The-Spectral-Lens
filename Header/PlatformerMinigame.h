#pragma once
#include "Minigame.h"
#include "Camera.h"
#include "Model.h"
#include <vector>
#include <glm/glm.hpp>

// Platform types
enum class PlatformType {
    SOLID,      // Regular platform
    MOVING,     // Platform that moves back and forth
    GOAL,       // Goal item/platform
    START,      // Starting platform (uses WinterTrees model)
    FINAL       // Final platform (uses SciFi model)
};

// Platform structure
struct Platform {
    glm::vec3 position;
    glm::vec3 size;        // width, height, depth
    PlatformType type;
    bool active;
    
    // For moving platforms
    glm::vec3 moveStart;
    glm::vec3 moveEnd;
    float moveSpeed;
    bool movingForward;
};

class PlatformerMinigame : public Minigame {
public:
    PlatformerMinigame(int width, int height);
    ~PlatformerMinigame();

    void update(float deltaTime) override;
    void render() override;
    bool checkWinCondition() override;
    bool isFinished() override;

private:
    // Screen dimensions
    int screenWidth, screenHeight;

    // Game state
    bool isGameOver;
    bool playerWon;
    bool waitingToStart;  // Freeze game until player presses space
    bool spaceWasPressed; // Track space key state for edge detection
    float timeElapsed;

    // Player physics
    float playerX, playerY, playerZ;
    float velocityX, velocityY, velocityZ;
    bool isGrounded;
    bool hasDoubleJump;
    
    // Player rotation (facing direction)
    float playerRotation;       // Current rotation angle (in radians)
    float targetRotation;       // Target rotation angle (in radians)
    const float ROTATION_SPEED = 10.0f; // How fast to rotate

    // Constants
    const float GRAVITY = -20.0f;
    const float JUMP_FORCE = 8.0f;
    const float MOVE_SPEED = 5.0f;
    const float DOUBLE_JUMP_FORCE = 7.0f;
    const float PLAYER_HEIGHT = 1.0f;
    const float PLAYER_RADIUS = 0.4f;

    // Rendering
    unsigned int shaderProgram;
    unsigned int VAO, VBO;  // For platform cubes
    Camera camera;
    Model playerModel;

    // Platform models
    Model startPlatformModel;   // WinterTrees - starting platform
    Model movingPlatformModel;  // Tiramisu - moving platforms
    Model goalPlatformModel;    // SciFi - final platform

    // HUD for "Press Space to Start" indicator
    unsigned int hudShaderProgram;
    unsigned int hudVAO, hudVBO;
    unsigned int fontTexture;

    // Level data
    std::vector<Platform> platforms;
    glm::vec3 goalPosition;
    float goalCollectRadius;

    // Setup functions
    void initializePlatformMap();
    void checkCollisions();
    bool checkPlatformCollision(const Platform& platform, glm::vec3& normal);
    void updateMovingPlatforms(float deltaTime);
    void renderPlatform(const Platform& platform);
    void renderCube(glm::mat4 model, glm::vec3 color);
    
    // HUD rendering
    void initializeHUD();
    void renderStartPrompt();
    void renderSkybox();

    // Skybox
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int skyboxTexture;
};
