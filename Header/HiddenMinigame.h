#pragma once
#include "Minigame.h"
#include "Camera.h"
#include "Model.h"
#include <vector>
#include <glm/glm.hpp>

// Types of objects in the scene
enum class ObjectType {
    GROUND,
    HOUSE_WALL,
    HOUSE_ROOF,
    TREE_TRUNK,
    TREE_LEAVES,
    ROCK,
    FENCE,
    CAT_HIDING  // The hidden cat to find
};

// Scene object structure
struct SceneObject {
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 rotation;  // Euler angles in degrees
    glm::vec3 color;
    ObjectType type;
    bool hasCollision = true;  // Whether this object blocks player movement
};

class HiddenMinigame : public Minigame {
public:
    HiddenMinigame(int width, int height);
    ~HiddenMinigame();

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
    float timeLimit;  // Time limit to find the cat

    // Player position and movement (first person)
    glm::vec3 playerPos;
    float playerYaw;    // Horizontal rotation (mouse X)
    float playerPitch;  // Vertical rotation (mouse Y)
    
    // Mouse state for first-person look
    float lastMouseX, lastMouseY;
    bool firstMouse;

    // Flashlight/Lens state
    bool lensActive;
    
    // Cursor lock state
    bool cursorLocked;
    
    // Movement constants
    const float MOVE_SPEED = 5.0f;
    const float MOUSE_SENSITIVITY = 0.1f;
    const float PLAYER_HEIGHT = 1.7f;
    const float PLAYER_RADIUS = 0.4f;  // Collision radius for player

    // Fog settings
    const float FOG_DENSITY_NORMAL = 0.15f;   // Heavy fog normally
    const float FOG_DENSITY_LENS = 0.02f;     // Light fog with lens
    const glm::vec3 FOG_COLOR = glm::vec3(0.1f, 0.1f, 0.15f);  // Dark blue-grey fog

    // Rendering
    unsigned int shaderProgram;
    unsigned int fogShaderProgram;
    unsigned int VAO, VBO;
    Camera camera;

    // Cat model (the hidden target)
    Model catModel;
    glm::vec3 catPosition;
    float catFindRadius;
    bool catFound;

    // Level models
    Model houseModel;
    Model treeModel;
    glm::vec3 housePosition;
    float houseScale;

    // Tree positions and scales for rendering
    struct TreeInstance {
        glm::vec3 position;
        float scale;
        float rotationY;  // Random Y rotation for variety
    };
    std::vector<TreeInstance> trees;

    // Scene objects (for collision boxes)
    std::vector<SceneObject> sceneObjects;

    // Map boundaries
    const float MAP_SIZE = 60.0f;

    // Setup functions
    void initializeScene();
    void placeCatRandomly();

    // Rendering functions
    void renderObject(const SceneObject& obj);
    void renderCube(glm::mat4 model, glm::vec3 color);
    void renderCat();
    void renderHouse();
    void renderTrees();
    void renderGround();
    void renderHUD();

    // Collision detection
    bool checkCollision(glm::vec3 newPos);
    bool checkAABBCollision(glm::vec3 playerPos, float playerRadius, const SceneObject& obj);
    
    // Input handling
    void processMouseMovement(float xoffset, float yoffset);
};
