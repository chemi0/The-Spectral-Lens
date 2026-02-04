#include "../Header/HiddenMinigame.h"
#include "../Header/InputManager.h"
#include "../Header/Util.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

HiddenMinigame::HiddenMinigame(int width, int height)
    : screenWidth(width), screenHeight(height),
    isGameOver(false), playerWon(false), waitingToStart(true), spaceWasPressed(true),
    timeElapsed(0.0f), timeLimit(90.0f),
    playerPos(0.0f, 1.7f, 0.0f),
    playerYaw(-90.0f), playerPitch(0.0f),
    lastMouseX(width / 2.0f), lastMouseY(height / 2.0f), firstMouse(true),
    lensActive(false),
    camera(glm::vec3(0.0f, 1.7f, 0.0f)),
    catFindRadius(2.5f), catFound(false),
    cursorLocked(false)
{
    srand(static_cast<unsigned int>(time(nullptr)));

    shaderProgram = createShader("Shaders/basic_3d.vert", "Shaders/basic_3d.frag");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

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

    if (!catModel.loadModel("Resources/RunnerCharacter/cat_with_lights.obj")) {
        std::cerr << "Failed to load cat model for HiddenMinigame!" << std::endl;
    }

    catModel.diffuseTex = catModel.loadTexture("Resources/RunnerCharacter/cat_diffuse.png");
    catModel.emissionTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Emissive.png");
    catModel.roughnessTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Roughness.png");
    catModel.aoTex = catModel.loadTexture("Resources/RunnerCharacter/cat_AmbientOcclusion.png");
    catModel.opacityTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Opacity.png");
    catModel.normalMapTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Normal.png");

    initializeScene();

    std::cout << "Hidden Minigame Initialized!" << std::endl;
    std::cout << "Press SPACE to start!" << std::endl;
    std::cout << "Controls: WASD = Move, Mouse = Look, Hold E = Use Lens/Flashlight" << std::endl;
    std::cout << "Find the hidden cat before time runs out!" << std::endl;
}

HiddenMinigame::~HiddenMinigame() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void HiddenMinigame::initializeScene() {
    sceneObjects.clear();

    SceneObject ground;
    ground.position = glm::vec3(0.0f, -0.25f, 0.0f);
    ground.size = glm::vec3(MAP_SIZE * 2, 0.5f, MAP_SIZE * 2);
    ground.rotation = glm::vec3(0.0f);
    ground.color = glm::vec3(0.15f, 0.25f, 0.1f);
    ground.type = ObjectType::GROUND;
    sceneObjects.push_back(ground);

    createHouse(glm::vec3(0.0f, 0.0f, -15.0f));

    createTree(glm::vec3(-20.0f, 0.0f, -10.0f), 1.2f);
    createTree(glm::vec3(-25.0f, 0.0f, -5.0f), 0.9f);
    createTree(glm::vec3(-22.0f, 0.0f, 0.0f), 1.0f);
    createTree(glm::vec3(-18.0f, 0.0f, 5.0f), 1.1f);
    createTree(glm::vec3(-24.0f, 0.0f, -18.0f), 1.3f);
    createTree(glm::vec3(-28.0f, 0.0f, -12.0f), 0.8f);
    createTree(glm::vec3(-15.0f, 0.0f, -20.0f), 1.0f);

    createTree(glm::vec3(20.0f, 0.0f, -8.0f), 1.1f);
    createTree(glm::vec3(25.0f, 0.0f, -15.0f), 1.0f);
    createTree(glm::vec3(22.0f, 0.0f, -22.0f), 0.9f);
    createTree(glm::vec3(18.0f, 0.0f, 8.0f), 1.2f);
    createTree(glm::vec3(28.0f, 0.0f, 0.0f), 1.0f);
    createTree(glm::vec3(15.0f, 0.0f, -25.0f), 1.1f);

    createTree(glm::vec3(-8.0f, 0.0f, -30.0f), 1.0f);
    createTree(glm::vec3(0.0f, 0.0f, -35.0f), 1.2f);
    createTree(glm::vec3(10.0f, 0.0f, -32.0f), 0.9f);
    createTree(glm::vec3(-5.0f, 0.0f, -40.0f), 1.1f);
    createTree(glm::vec3(8.0f, 0.0f, -38.0f), 1.0f);

    createTree(glm::vec3(-12.0f, 0.0f, 15.0f), 1.0f);
    createTree(glm::vec3(14.0f, 0.0f, 18.0f), 0.8f);
    createTree(glm::vec3(5.0f, 0.0f, 25.0f), 1.1f);

    createRock(glm::vec3(-10.0f, 0.0f, -5.0f), 1.5f);
    createRock(glm::vec3(12.0f, 0.0f, -3.0f), 1.0f);
    createRock(glm::vec3(-5.0f, 0.0f, 10.0f), 0.8f);
    createRock(glm::vec3(8.0f, 0.0f, -20.0f), 1.2f);
    createRock(glm::vec3(-18.0f, 0.0f, -25.0f), 1.0f);
    createRock(glm::vec3(22.0f, 0.0f, 5.0f), 0.9f);
    createRock(glm::vec3(-8.0f, 0.0f, -35.0f), 1.3f);
    createRock(glm::vec3(15.0f, 0.0f, -30.0f), 1.1f);

    createRock(glm::vec3(-3.0f, 0.0f, 5.0f), 0.5f);
    createRock(glm::vec3(-2.0f, 0.0f, 6.0f), 0.4f);
    createRock(glm::vec3(7.0f, 0.0f, 12.0f), 0.6f);
    createRock(glm::vec3(8.5f, 0.0f, 11.0f), 0.5f);

    createFence(glm::vec3(-15.0f, 0.0f, 5.0f), glm::vec3(-3.0f, 0.0f, 5.0f));
    createFence(glm::vec3(3.0f, 0.0f, 5.0f), glm::vec3(15.0f, 0.0f, 5.0f));
    createFence(glm::vec3(-15.0f, 0.0f, 5.0f), glm::vec3(-15.0f, 0.0f, -25.0f));
    createFence(glm::vec3(15.0f, 0.0f, 5.0f), glm::vec3(15.0f, 0.0f, -25.0f));
    createFence(glm::vec3(-15.0f, 0.0f, -25.0f), glm::vec3(15.0f, 0.0f, -25.0f));

    placeCatRandomly();

    playerPos = glm::vec3(0.0f, PLAYER_HEIGHT, 20.0f);
}

void HiddenMinigame::createHouse(glm::vec3 position) {
    SceneObject frontWall;
    frontWall.position = position + glm::vec3(0.0f, 2.0f, 4.0f);
    frontWall.size = glm::vec3(10.0f, 4.0f, 0.3f);
    frontWall.rotation = glm::vec3(0.0f);
    frontWall.color = glm::vec3(0.6f, 0.5f, 0.4f);
    frontWall.type = ObjectType::HOUSE_WALL;
    sceneObjects.push_back(frontWall);

    SceneObject backWall;
    backWall.position = position + glm::vec3(0.0f, 2.0f, -4.0f);
    backWall.size = glm::vec3(10.0f, 4.0f, 0.3f);
    backWall.rotation = glm::vec3(0.0f);
    backWall.color = glm::vec3(0.6f, 0.5f, 0.4f);
    backWall.type = ObjectType::HOUSE_WALL;
    sceneObjects.push_back(backWall);

    SceneObject leftWall;
    leftWall.position = position + glm::vec3(-5.0f, 2.0f, 0.0f);
    leftWall.size = glm::vec3(0.3f, 4.0f, 8.0f);
    leftWall.rotation = glm::vec3(0.0f);
    leftWall.color = glm::vec3(0.55f, 0.45f, 0.35f);
    leftWall.type = ObjectType::HOUSE_WALL;
    sceneObjects.push_back(leftWall);

    SceneObject rightWall;
    rightWall.position = position + glm::vec3(5.0f, 2.0f, 0.0f);
    rightWall.size = glm::vec3(0.3f, 4.0f, 8.0f);
    rightWall.rotation = glm::vec3(0.0f);
    rightWall.color = glm::vec3(0.55f, 0.45f, 0.35f);
    rightWall.type = ObjectType::HOUSE_WALL;
    sceneObjects.push_back(rightWall);

    SceneObject roofLeft;
    roofLeft.position = position + glm::vec3(-2.5f, 5.0f, 0.0f);
    roofLeft.size = glm::vec3(6.0f, 0.3f, 9.0f);
    roofLeft.rotation = glm::vec3(0.0f, 0.0f, 25.0f);
    roofLeft.color = glm::vec3(0.4f, 0.2f, 0.15f);
    roofLeft.type = ObjectType::HOUSE_ROOF;
    sceneObjects.push_back(roofLeft);

    SceneObject roofRight;
    roofRight.position = position + glm::vec3(2.5f, 5.0f, 0.0f);
    roofRight.size = glm::vec3(6.0f, 0.3f, 9.0f);
    roofRight.rotation = glm::vec3(0.0f, 0.0f, -25.0f);
    roofRight.color = glm::vec3(0.4f, 0.2f, 0.15f);
    roofRight.type = ObjectType::HOUSE_ROOF;
    sceneObjects.push_back(roofRight);

    SceneObject door;
    door.position = position + glm::vec3(0.0f, 1.2f, 4.2f);
    door.size = glm::vec3(1.5f, 2.4f, 0.1f);
    door.rotation = glm::vec3(0.0f);
    door.color = glm::vec3(0.3f, 0.2f, 0.1f);
    door.type = ObjectType::HOUSE_WALL;
    sceneObjects.push_back(door);

    SceneObject windowLeft;
    windowLeft.position = position + glm::vec3(-2.5f, 2.5f, 4.2f);
    windowLeft.size = glm::vec3(1.2f, 1.0f, 0.1f);
    windowLeft.rotation = glm::vec3(0.0f);
    windowLeft.color = glm::vec3(0.3f, 0.4f, 0.5f);
    windowLeft.type = ObjectType::HOUSE_WALL;
    sceneObjects.push_back(windowLeft);

    SceneObject windowRight;
    windowRight.position = position + glm::vec3(2.5f, 2.5f, 4.2f);
    windowRight.size = glm::vec3(1.2f, 1.0f, 0.1f);
    windowRight.rotation = glm::vec3(0.0f);
    windowRight.color = glm::vec3(0.3f, 0.4f, 0.5f);
    windowRight.type = ObjectType::HOUSE_WALL;
    sceneObjects.push_back(windowRight);
}

void HiddenMinigame::createTree(glm::vec3 position, float scale) {
    SceneObject trunk;
    trunk.position = position + glm::vec3(0.0f, 1.5f * scale, 0.0f);
    trunk.size = glm::vec3(0.5f * scale, 3.0f * scale, 0.5f * scale);
    trunk.rotation = glm::vec3(0.0f);
    trunk.color = glm::vec3(0.35f, 0.2f, 0.1f);
    trunk.type = ObjectType::TREE_TRUNK;
    sceneObjects.push_back(trunk);

    SceneObject leaves1;
    leaves1.position = position + glm::vec3(0.0f, 4.0f * scale, 0.0f);
    leaves1.size = glm::vec3(3.0f * scale, 2.5f * scale, 3.0f * scale);
    leaves1.rotation = glm::vec3(0.0f);
    leaves1.color = glm::vec3(0.1f, 0.35f, 0.1f);
    leaves1.type = ObjectType::TREE_LEAVES;
    sceneObjects.push_back(leaves1);

    SceneObject leaves2;
    leaves2.position = position + glm::vec3(0.0f, 5.5f * scale, 0.0f);
    leaves2.size = glm::vec3(2.0f * scale, 2.0f * scale, 2.0f * scale);
    leaves2.rotation = glm::vec3(0.0f, 45.0f, 0.0f);
    leaves2.color = glm::vec3(0.15f, 0.4f, 0.12f);
    leaves2.type = ObjectType::TREE_LEAVES;
    sceneObjects.push_back(leaves2);
}

void HiddenMinigame::createRock(glm::vec3 position, float scale) {
    SceneObject rock;
    rock.position = position + glm::vec3(0.0f, 0.3f * scale, 0.0f);
    rock.size = glm::vec3(1.0f * scale, 0.6f * scale, 0.8f * scale);
    rock.rotation = glm::vec3(0.0f, static_cast<float>(rand() % 360), 0.0f);
    rock.color = glm::vec3(0.4f, 0.4f, 0.42f);
    rock.type = ObjectType::ROCK;
    sceneObjects.push_back(rock);
}

void HiddenMinigame::createFence(glm::vec3 start, glm::vec3 end) {
    glm::vec3 direction = end - start;
    float length = glm::length(direction);
    glm::vec3 midpoint = (start + end) * 0.5f;
    float angle = atan2(direction.x, direction.z);

    int numPosts = static_cast<int>(length / 2.0f) + 1;
    for (int i = 0; i < numPosts; i++) {
        float t = static_cast<float>(i) / static_cast<float>(numPosts - 1);
        glm::vec3 postPos = start + direction * t;
        
        SceneObject post;
        post.position = postPos + glm::vec3(0.0f, 0.5f, 0.0f);
        post.size = glm::vec3(0.15f, 1.0f, 0.15f);
        post.rotation = glm::vec3(0.0f);
        post.color = glm::vec3(0.45f, 0.3f, 0.2f);
        post.type = ObjectType::FENCE;
        sceneObjects.push_back(post);
    }

    SceneObject rail1;
    rail1.position = midpoint + glm::vec3(0.0f, 0.3f, 0.0f);
    rail1.size = glm::vec3(0.1f, 0.1f, length);
    rail1.rotation = glm::vec3(0.0f, glm::degrees(angle), 0.0f);
    rail1.color = glm::vec3(0.5f, 0.35f, 0.25f);
    rail1.type = ObjectType::FENCE;
    sceneObjects.push_back(rail1);

    SceneObject rail2;
    rail2.position = midpoint + glm::vec3(0.0f, 0.7f, 0.0f);
    rail2.size = glm::vec3(0.1f, 0.1f, length);
    rail2.rotation = glm::vec3(0.0f, glm::degrees(angle), 0.0f);
    rail2.color = glm::vec3(0.5f, 0.35f, 0.25f);
    rail2.type = ObjectType::FENCE;
    sceneObjects.push_back(rail2);
}

void HiddenMinigame::placeCatRandomly() {
    std::vector<glm::vec3> hidingSpots = {
        glm::vec3(-22.0f, 0.0f, -3.0f),
        glm::vec3(23.0f, 0.0f, -10.0f),
        glm::vec3(-5.0f, 0.0f, -19.0f),
        glm::vec3(8.0f, 0.0f, -19.0f),
        glm::vec3(-14.0f, 0.0f, -5.0f),
        glm::vec3(14.0f, 0.0f, -20.0f),
        glm::vec3(-10.0f, 0.0f, -6.0f),
        glm::vec3(12.0f, 0.0f, -4.0f),
        glm::vec3(-25.0f, 0.0f, -15.0f),
        glm::vec3(26.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, -38.0f),
        glm::vec3(-8.0f, 0.0f, -32.0f),
    };

    int spotIndex = rand() % hidingSpots.size();
    catPosition = hidingSpots[spotIndex];
    catPosition.y = 0.0f;

    std::cout << "Cat hidden at position: (" << catPosition.x << ", " << catPosition.z << ")" << std::endl;
}

void HiddenMinigame::update(float deltaTime) {
    if (isGameOver) {
        // Unlock cursor when game is over
        if (cursorLocked) {
            GLFWwindow* window = glfwGetCurrentContext();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursorLocked = false;
        }
        return;
    }

    InputManager& input = InputManager::getInstance();
    bool spaceIsPressed = input.isKeyDown(GLFW_KEY_SPACE);

    if (waitingToStart) {
        timeElapsed += deltaTime;
        
        if (!spaceIsPressed) {
            spaceWasPressed = false;
        }
        if (spaceIsPressed && !spaceWasPressed) {
            waitingToStart = false;
            spaceWasPressed = true;
            timeElapsed = 0.0f;
            
            // Lock cursor when game starts
            GLFWwindow* window = glfwGetCurrentContext();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cursorLocked = true;
            firstMouse = true;  // Reset mouse tracking
            
            std::cout << "Game Started! Find the cat!" << std::endl;
        }
        return;
    }

    timeElapsed += deltaTime;

    if (timeElapsed >= timeLimit) {
        isGameOver = true;
        playerWon = false;
        
        // Unlock cursor on game over
        GLFWwindow* window = glfwGetCurrentContext();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorLocked = false;
        
        std::cout << "TIME'S UP! You failed to find the cat." << std::endl;
        return;
    }

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
        glm::vec3 newPos = playerPos + movement;
        
        newPos.x = glm::clamp(newPos.x, -MAP_SIZE + 1.0f, MAP_SIZE - 1.0f);
        newPos.z = glm::clamp(newPos.z, -MAP_SIZE + 1.0f, MAP_SIZE - 1.0f);
        
        playerPos = newPos;
    }

    playerPos.y = PLAYER_HEIGHT;

    lensActive = input.isKeyDown(GLFW_KEY_E);

    float distToCat = glm::length(glm::vec2(playerPos.x - catPosition.x, playerPos.z - catPosition.z));
    
    if (distToCat < catFindRadius && lensActive) {
        if (input.leftMousePressed) {
            catFound = true;
            isGameOver = true;
            playerWon = true;
            
            // Unlock cursor on victory
            GLFWwindow* window = glfwGetCurrentContext();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursorLocked = false;
            
            std::cout << "YOU FOUND THE CAT! Victory!" << std::endl;
        }
    }

    camera.Position = playerPos;
    camera.Yaw = playerYaw;
    camera.Pitch = playerPitch;
}

void HiddenMinigame::processMouseMovement(float xoffset, float yoffset) {
    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;

    playerYaw += xoffset;
    playerPitch += yoffset;

    if (playerPitch > 89.0f) playerPitch = 89.0f;
    if (playerPitch < -89.0f) playerPitch = -89.0f;
}

void HiddenMinigame::render() {
    float fogDensity = lensActive ? FOG_DENSITY_LENS : FOG_DENSITY_NORMAL;
    glClearColor(FOG_COLOR.r, FOG_COLOR.g, FOG_COLOR.b, 1.0f);

    glUseProgram(shaderProgram);

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    
    glm::vec3 front;
    front.x = cos(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front.y = sin(glm::radians(playerPitch));
    front.z = sin(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front = glm::normalize(front);
    
    glm::mat4 view = glm::lookAt(playerPos, playerPos + front, glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(playerPos));

    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), fogDensity);
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(FOG_COLOR));
    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 1);

    for (const auto& obj : sceneObjects) {
        renderObject(obj);
    }

    renderCat();
    renderHUD();
}

void HiddenMinigame::renderObject(const SceneObject& obj) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, obj.position);
    
    if (obj.rotation.x != 0.0f)
        model = glm::rotate(model, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    if (obj.rotation.y != 0.0f)
        model = glm::rotate(model, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    if (obj.rotation.z != 0.0f)
        model = glm::rotate(model, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    model = glm::scale(model, obj.size);

    renderCube(model, obj.color);
}

void HiddenMinigame::renderCube(glm::mat4 model, glm::vec3 color) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, glm::value_ptr(color));
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0);

    glBindVertexArray(VAO);
    glDisableVertexAttribArray(3);
    glVertexAttrib3f(3, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void HiddenMinigame::renderCat() {
    glm::mat4 catMatrix = glm::mat4(1.0f);
    catMatrix = glm::translate(catMatrix, catPosition);
    catMatrix = glm::scale(catMatrix, glm::vec3(0.002f));  // Much smaller cat!
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(catMatrix));

    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);

    catModel.render();
}

void HiddenMinigame::renderHUD() {
    // HUD rendering would go here (timer display, etc.)
}

bool HiddenMinigame::checkCollision(glm::vec3 newPos) {
    return false;
}

bool HiddenMinigame::checkWinCondition() {
    return playerWon;
}

bool HiddenMinigame::isFinished() {
    return isGameOver;
}
