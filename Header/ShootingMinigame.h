#pragma once
#include "Minigame.h"
#include "Camera.h"
#include "Model.h"
#include <vector>
#include <glm/glm.hpp>

// Projectile structure
struct Projectile {
    glm::vec3 position;
    glm::vec3 velocity;
    bool active;
};

// Cat target in the sky
struct CatTarget {
    glm::vec3 position;
    bool alive;
    bool isReal;       // Only one cat is the "real" target
    float popTimer;    // Animation timer for pop effect
    bool popping;      // Currently playing pop animation
};

class ShootingMinigame : public Minigame {
public:
    ShootingMinigame(int width, int height);
    ~ShootingMinigame();

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
    bool waitingToStart;
    bool spaceWasPressed;
    float timeElapsed;
    float gameTimer;

    // Timer & win display
    static constexpr float TIME_LIMIT = 60.0f;
    static constexpr float NAME_DISPLAY_TIME = 5.0f;
    bool showingName;
    float nameDisplayTimer;

    // Player / first-person camera
    glm::vec3 playerPos;
    float playerYaw;
    float playerPitch;
    float lastMouseX, lastMouseY;
    bool firstMouse;
    bool cursorLocked;

    // Movement constants
    static constexpr float MOUSE_SENSITIVITY = 0.1f;
    static constexpr float PLAYER_HEIGHT = 1.7f;
    static constexpr float MOVE_SPEED = 5.0f;

    // Slingshot charging
    bool isCharging;
    float chargeTime;
    static constexpr float MAX_CHARGE_TIME = 2.0f;
    static constexpr float MAX_PROJECTILE_SPEED = 30.0f;

    // Projectiles
    std::vector<Projectile> projectiles;
    static constexpr float PROJECTILE_GRAVITY = -9.81f;
    static constexpr float PROJECTILE_RADIUS = 0.3f;

    // Cat targets
    std::vector<CatTarget> cats;
    static constexpr float CAT_HIT_RADIUS = 1.5f;
    static constexpr float CAT_SPAWN_HEIGHT_MIN = 15.0f;
    static constexpr float CAT_SPAWN_HEIGHT_MAX = 25.0f;
    static constexpr float CAT_SPAWN_RADIUS = 20.0f;
    static constexpr float CAT_SCALE = 0.01f;
    static constexpr float POP_DURATION = 0.5f;

    // Rendering
    unsigned int shaderProgram;
    unsigned int VAO, VBO;
    Camera camera;

    // Models
    Model slingshotModel;
    Model catModel;
    Model pumpkinModel;
    Model groundModel;

    // Skybox
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int skyboxTexture;

    // HUD
    unsigned int hudShaderProgram;
    unsigned int hudVAO, hudVBO;
    unsigned int fontTexture;

    // Name texture for billboard
    unsigned int nameTexture;
    unsigned int nameVAO, nameVBO;

    // Setup functions
    void initializeScene();
    void spawnCats();

    // Update functions
    void updateProjectiles(float deltaTime);
    void checkProjectileHits();

    // Rendering functions
    void renderGround();
    void renderCats();
    void renderProjectiles();
    void renderSlingshot();
    void renderCrosshair();
    void renderHUD();
    void renderNameBillboard();
    void renderCube(glm::mat4 model, glm::vec3 color);
    void renderSkybox();
    void renderLetter(char letter, glm::vec3 position, float size, glm::vec3 color);
    void renderText(const std::string& text, glm::vec3 startPos, float size, float spacing, glm::vec3 color);

    // Input handling
    void processMouseMovement(float xoffset, float yoffset);

    // HUD setup
    void initializeHUD();
};
