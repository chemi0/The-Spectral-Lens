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
    cursorLocked(false),
    houseScale(0.2f),
    housePosition(0.0f, 0.0f, -15.0f)
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

    // Load cat model
    if (!catModel.loadModel("Resources/RunnerCharacter/cat_with_lights.obj")) {
        std::cerr << "Failed to load cat model for HiddenMinigame!" << std::endl;
    }
    catModel.diffuseTex = catModel.loadTexture("Resources/RunnerCharacter/cat_diffuse.png");
    catModel.emissionTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Emissive.png");
    catModel.roughnessTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Roughness.png");
    catModel.aoTex = catModel.loadTexture("Resources/RunnerCharacter/cat_AmbientOcclusion.png");
    catModel.opacityTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Opacity.png");
    catModel.normalMapTex = catModel.loadTexture("Resources/RunnerCharacter/cat_Normal.png");

    // Load house model
    if (!houseModel.loadModel("Resources/HidingGameHouse/Low_poly_house.obj")) {
        std::cerr << "Failed to load house model for HiddenMinigame!" << std::endl;
    }
    houseModel.diffuseTex = houseModel.loadTexture("Resources/HidingGameHouse/Low_poly_house.jpg");
    houseModel.emissionTex = houseModel.loadTexture("Resources/HidingGameHouse/Low_poly_house_emis.jpg");

    // Load tree model
    if (!treeModel.loadModel("Resources/HidingGameTree2/tree2.obj")) {
        std::cerr << "Failed to load tree model for HiddenMinigame!" << std::endl;
    }
    treeModel.diffuseTex = treeModel.loadTexture("Resources/HidingGameTree2/tree2Color.png");
    treeModel.normalMapTex = treeModel.loadTexture("Resources/HidingGameTree2/tree2Normal.png");
    treeModel.roughnessTex = treeModel.loadTexture("Resources/HidingGameTree2/tree2Roughness.png");

    initializeScene();

    std::cout << "Hidden Minigame Initialized!" << std::endl;
    std::cout << "Press SPACE to start!" << std::endl;
    std::cout << "Controls: WASD = Move, Mouse = Look, Hold E = Use Lens/Flashlight" << std::endl;
    std::cout << "Find the hidden cat before time runs out!" << std::endl;
    std::cout << "Use your lens to reveal what's hidden in the darkness..." << std::endl;
}

HiddenMinigame::~HiddenMinigame() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void HiddenMinigame::initializeScene() {
    sceneObjects.clear();
    trees.clear();

    // House placement (raised above ground to avoid z-fighting)
    housePosition = glm::vec3(0.0f, 0.02f, -15.0f);
    houseScale = 0.2f;

    // House collision box (adjusted for 0.2 scale, slightly larger to prevent wall clipping)
    SceneObject houseCollision;
    houseCollision.position = glm::vec3(0.0f, 3.0f, -15.0f);
    houseCollision.size = glm::vec3(12.0f, 6.0f, 10.0f);  // Slightly larger than visual model
    houseCollision.rotation = glm::vec3(0.0f);
    houseCollision.type = ObjectType::HOUSE_WALL;
    houseCollision.hasCollision = true;
    sceneObjects.push_back(houseCollision);

    // Ground (visual, no collision needed)
    SceneObject ground;
    ground.position = glm::vec3(0.0f, -0.25f, 0.0f);
    ground.size = glm::vec3(MAP_SIZE * 2, 0.5f, MAP_SIZE * 2);
    ground.rotation = glm::vec3(0.0f);
    ground.color = glm::vec3(0.12f, 0.22f, 0.08f);
    ground.type = ObjectType::GROUND;
    ground.hasCollision = false;
    sceneObjects.push_back(ground);

    // --- Tree placement ---
    // Left forest cluster
    trees.push_back({ glm::vec3(-20.0f, 0.0f, -10.0f), 0.08f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3(-25.0f, 0.0f,  -5.0f), 0.06f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3(-22.0f, 0.0f,   0.0f), 0.07f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3(-18.0f, 0.0f,   5.0f), 0.075f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3(-24.0f, 0.0f, -18.0f), 0.09f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3(-28.0f, 0.0f, -12.0f), 0.055f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3(-15.0f, 0.0f, -20.0f), 0.07f, static_cast<float>(rand() % 360) });

    // Right forest cluster
    trees.push_back({ glm::vec3( 20.0f, 0.0f,  -8.0f), 0.075f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3( 25.0f, 0.0f, -15.0f), 0.07f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3( 22.0f, 0.0f, -22.0f), 0.065f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3( 18.0f, 0.0f,   8.0f), 0.08f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3( 28.0f, 0.0f,   0.0f), 0.07f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3( 15.0f, 0.0f, -25.0f), 0.075f, static_cast<float>(rand() % 360) });

    // Behind the house
    trees.push_back({ glm::vec3( -8.0f, 0.0f, -30.0f), 0.07f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3(  0.0f, 0.0f, -35.0f), 0.08f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3( 10.0f, 0.0f, -32.0f), 0.065f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3( -5.0f, 0.0f, -40.0f), 0.075f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3(  8.0f, 0.0f, -38.0f), 0.07f, static_cast<float>(rand() % 360) });

    // In front / scattered (away from spawn point at z=25)
    trees.push_back({ glm::vec3(-12.0f, 0.0f,  12.0f), 0.07f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3( 14.0f, 0.0f,  10.0f), 0.055f, static_cast<float>(rand() % 360) });
    trees.push_back({ glm::vec3( -8.0f, 0.0f,  28.0f), 0.075f, static_cast<float>(rand() % 360) });

    // Collision boxes for tree trunks (larger to prevent walking through)
    for (const auto& tree : trees) {
        SceneObject trunkCollision;
        trunkCollision.position = tree.position + glm::vec3(0.0f, 2.5f, 0.0f);
        trunkCollision.size = glm::vec3(2.4f, 5.0f, 2.4f);  // Larger trunk collision
        trunkCollision.rotation = glm::vec3(0.0f);
        trunkCollision.type = ObjectType::TREE_TRUNK;
        trunkCollision.hasCollision = true;
        sceneObjects.push_back(trunkCollision);
    }

    // Fence collision boxes (invisible, just to bound the play area)
    // Left fence
    SceneObject fenceLeft;
    fenceLeft.position = glm::vec3(-15.0f, 0.5f, -10.0f);
    fenceLeft.size = glm::vec3(0.3f, 1.0f, 30.0f);
    fenceLeft.rotation = glm::vec3(0.0f);
    fenceLeft.type = ObjectType::FENCE;
    fenceLeft.hasCollision = true;
    sceneObjects.push_back(fenceLeft);

    // Right fence
    SceneObject fenceRight;
    fenceRight.position = glm::vec3(15.0f, 0.5f, -10.0f);
    fenceRight.size = glm::vec3(0.3f, 1.0f, 30.0f);
    fenceRight.rotation = glm::vec3(0.0f);
    fenceRight.type = ObjectType::FENCE;
    fenceRight.hasCollision = true;
    sceneObjects.push_back(fenceRight);

    // Back fence
    SceneObject fenceBack;
    fenceBack.position = glm::vec3(0.0f, 0.5f, -25.0f);
    fenceBack.size = glm::vec3(30.0f, 1.0f, 0.3f);
    fenceBack.rotation = glm::vec3(0.0f);
    fenceBack.type = ObjectType::FENCE;
    fenceBack.hasCollision = true;
    sceneObjects.push_back(fenceBack);

    // Front fence (two parts with gap for entrance)
    SceneObject fenceFrontLeft;
    fenceFrontLeft.position = glm::vec3(-9.0f, 0.5f, 5.0f);
    fenceFrontLeft.size = glm::vec3(12.0f, 1.0f, 0.3f);
    fenceFrontLeft.rotation = glm::vec3(0.0f);
    fenceFrontLeft.type = ObjectType::FENCE;
    fenceFrontLeft.hasCollision = true;
    sceneObjects.push_back(fenceFrontLeft);

    SceneObject fenceFrontRight;
    fenceFrontRight.position = glm::vec3(9.0f, 0.5f, 5.0f);
    fenceFrontRight.size = glm::vec3(12.0f, 1.0f, 0.3f);
    fenceFrontRight.rotation = glm::vec3(0.0f);
    fenceFrontRight.type = ObjectType::FENCE;
    fenceFrontRight.hasCollision = true;
    sceneObjects.push_back(fenceFrontRight);

    placeCatRandomly();

    // Spawn player further in front, away from trees
    playerPos = glm::vec3(0.0f, PLAYER_HEIGHT, 27.0f);
}

void HiddenMinigame::placeCatRandomly() {
    // All hiding spots around the house at (0, 0, -15)
    std::vector<glm::vec3> hidingSpots = {
        // Left side of house
        glm::vec3(-10.0f, 0.0f, -15.0f),
        glm::vec3(-12.0f, 0.0f, -12.0f),
        glm::vec3(-11.0f, 0.0f, -18.0f),
        // Right side of house
        glm::vec3(10.0f, 0.0f, -15.0f),
        glm::vec3(12.0f, 0.0f, -13.0f),
        glm::vec3(11.0f, 0.0f, -17.0f),
        // Behind house
        glm::vec3(0.0f, 0.0f, -23.0f),
        glm::vec3(-6.0f, 0.0f, -22.0f),
        glm::vec3(6.0f, 0.0f, -22.0f),
        // In front of house (sides)
        glm::vec3(-8.0f, 0.0f, -8.0f),
        glm::vec3(8.0f, 0.0f, -8.0f),
    };

    int spotIndex = rand() % hidingSpots.size();
    catPosition = hidingSpots[spotIndex];
    catPosition.y = 0.0f;

    std::cout << "Cat hidden at position: (" << catPosition.x << ", " << catPosition.z << ")" << std::endl;
}

void HiddenMinigame::update(float deltaTime) {
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

    if (waitingToStart) {
        timeElapsed += deltaTime;

        if (!spaceIsPressed) {
            spaceWasPressed = false;
        }
        if (spaceIsPressed && !spaceWasPressed) {
            waitingToStart = false;
            spaceWasPressed = true;
            timeElapsed = 0.0f;

            GLFWwindow* window = glfwGetCurrentContext();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cursorLocked = true;
            firstMouse = true;

            std::cout << "Game Started! Find the cat!" << std::endl;
        }
        return;
    }

    timeElapsed += deltaTime;

    if (timeElapsed >= timeLimit) {
        isGameOver = true;
        playerWon = false;

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

        glm::vec3 testPosX = glm::vec3(newPos.x, playerPos.y, playerPos.z);
        glm::vec3 testPosZ = glm::vec3(playerPos.x, playerPos.y, newPos.z);

        if (!checkCollision(testPosX)) {
            playerPos.x = newPos.x;
        }
        if (!checkCollision(testPosZ)) {
            playerPos.z = newPos.z;
        }
    }

    playerPos.y = PLAYER_HEIGHT;

    lensActive = input.isKeyDown(GLFW_KEY_E);

    float distToCat = glm::length(glm::vec2(playerPos.x - catPosition.x, playerPos.z - catPosition.z));

    if (distToCat < catFindRadius && lensActive) {
        if (input.leftMousePressed) {
            catFound = true;
            isGameOver = true;
            playerWon = true;

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

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)screenWidth / (float)screenHeight, 0.1f, 200.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front.y = sin(glm::radians(playerPitch));
    front.z = sin(glm::radians(playerYaw)) * cos(glm::radians(playerPitch));
    front = glm::normalize(front);

    glm::mat4 view = glm::lookAt(playerPos, playerPos + front, glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(playerPos));

    // Set up directional light (sun/moonlight from above)
    glm::vec3 lightDir = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.5f));
    glm::vec3 lightColor = glm::vec3(0.6f, 0.65f, 0.7f);  // Dim bluish moonlight
    glm::vec3 ambientLight = glm::vec3(0.15f, 0.15f, 0.2f);  // Low ambient
    
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "ambientLight"), 1, glm::value_ptr(ambientLight));

    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), fogDensity);
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(FOG_COLOR));
    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 1);

    // Render ground
    renderGround();

    // Render house
    renderHouse();

    // Render all trees
    renderTrees();

    // Render the cat
    renderCat();

    // HUD on top
    renderHUD();
}

void HiddenMinigame::renderGround() {
    // Temporarily disable face culling for ground (it's a flat plane viewed from above)
    // This ensures ground is always visible regardless of global face culling toggle
    GLboolean wasCullingEnabled = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
    model = glm::scale(model, glm::vec3(MAP_SIZE * 2, 0.5f, MAP_SIZE * 2));
    renderCube(model, glm::vec3(0.12f, 0.22f, 0.08f));
    
    // Restore face culling state
    if (wasCullingEnabled) {
        glEnable(GL_CULL_FACE);
    }
}

void HiddenMinigame::renderHouse() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, housePosition);
    model = glm::scale(model, glm::vec3(houseScale));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);  // White tint for texture
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);

    houseModel.render();
}

void HiddenMinigame::renderTrees() {
    // Temporarily disable face culling for trees (leaves are flat planes that need double-sided rendering)
    GLboolean wasCullingEnabled = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    
    for (const auto& tree : trees) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, tree.position);
        model = glm::rotate(model, glm::radians(tree.rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(tree.scale));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
        glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);  // White tint for texture
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);

        treeModel.render();
    }
    
    // Restore face culling state
    if (wasCullingEnabled) {
        glEnable(GL_CULL_FACE);
    }
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
    catMatrix = glm::scale(catMatrix, glm::vec3(0.002f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(catMatrix));

    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f);  // White tint for texture
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_emission"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_opacity"), 4);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal"), 5);

    catModel.render();
}

void HiddenMinigame::renderHUD() {
    if (waitingToStart) return;

    float timeRemaining = timeLimit - timeElapsed;
    if (timeRemaining < 0.0f) timeRemaining = 0.0f;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    float barWidth = 0.8f;
    float barHeight = 0.05f;
    float barY = 0.9f;

    glm::mat4 orthoProj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 identityView = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(orthoProj));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(identityView));
    glUniform1i(glGetUniformLocation(shaderProgram, "useFog"), 0);

    glm::mat4 bgBar = glm::mat4(1.0f);
    bgBar = glm::translate(bgBar, glm::vec3(0.0f, barY, 0.0f));
    bgBar = glm::scale(bgBar, glm::vec3(barWidth, barHeight, 0.1f));
    renderCube(bgBar, glm::vec3(0.2f, 0.2f, 0.2f));

    float progress = timeRemaining / timeLimit;
    glm::mat4 progressBar = glm::mat4(1.0f);
    float progressWidth = barWidth * progress;
    float offsetX = -(barWidth - progressWidth) / 2.0f;
    progressBar = glm::translate(progressBar, glm::vec3(offsetX, barY, 0.1f));
    progressBar = glm::scale(progressBar, glm::vec3(progressWidth, barHeight * 0.8f, 0.1f));

    glm::vec3 timerColor;
    if (progress > 0.5f) {
        timerColor = glm::vec3(0.2f, 0.8f, 0.2f);
    } else if (progress > 0.25f) {
        timerColor = glm::vec3(0.9f, 0.7f, 0.1f);
    } else {
        timerColor = glm::vec3(0.9f, 0.2f, 0.2f);
    }
    renderCube(progressBar, timerColor);

    int secondsRemaining = static_cast<int>(timeRemaining);
    int numBlocks = 18;
    float blockStartX = -0.85f;
    float blockSpacing = 0.095f;

    for (int i = 0; i < numBlocks; i++) {
        glm::mat4 blockModel = glm::mat4(1.0f);
        blockModel = glm::translate(blockModel, glm::vec3(blockStartX + i * blockSpacing, barY - 0.1f, 0.0f));
        blockModel = glm::scale(blockModel, glm::vec3(0.07f, 0.04f, 0.1f));

        bool blockActive = (secondsRemaining > i * 5);
        glm::vec3 blockColor = blockActive ? timerColor : glm::vec3(0.15f, 0.15f, 0.15f);
        renderCube(blockModel, blockColor);
    }

    if (lensActive) {
        glm::mat4 lensIndicator = glm::mat4(1.0f);
        lensIndicator = glm::translate(lensIndicator, glm::vec3(0.0f, -0.85f, 0.0f));
        lensIndicator = glm::scale(lensIndicator, glm::vec3(0.15f, 0.05f, 0.1f));
        renderCube(lensIndicator, glm::vec3(0.2f, 0.8f, 1.0f));
    }

    glEnable(GL_DEPTH_TEST);
}

bool HiddenMinigame::checkCollision(glm::vec3 newPos) {
    for (const auto& obj : sceneObjects) {
        if (!obj.hasCollision) continue;

        if (checkAABBCollision(newPos, PLAYER_RADIUS, obj)) {
            return true;
        }
    }
    return false;
}

bool HiddenMinigame::checkAABBCollision(glm::vec3 playerPosition, float playerRadius, const SceneObject& obj) {
    glm::vec3 objMin = obj.position - obj.size * 0.5f;
    glm::vec3 objMax = obj.position + obj.size * 0.5f;

    objMin.x -= playerRadius;
    objMin.z -= playerRadius;
    objMax.x += playerRadius;
    objMax.z += playerRadius;

    float playerFeet = playerPosition.y - PLAYER_HEIGHT;
    float playerHead = playerPosition.y + 0.3f;

    bool xOverlap = playerPosition.x >= objMin.x && playerPosition.x <= objMax.x;
    bool zOverlap = playerPosition.z >= objMin.z && playerPosition.z <= objMax.z;
    bool yOverlap = playerHead >= objMin.y && playerFeet <= objMax.y;

    return xOverlap && zOverlap && yOverlap;
}

bool HiddenMinigame::checkWinCondition() {
    return playerWon;
}

bool HiddenMinigame::isFinished() {
    return isGameOver;
}
