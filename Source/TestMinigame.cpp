#include "../Header/TestMinigame.h"
#include <iostream>

TestMinigame::TestMinigame(int width, int height)
	: screenWidth(width), screenHeight(height), isGameOver(false), playerWon(false), timeElapsed(0.0f) {
	
	// Shader loading
	/*unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, "Shaders/basic_3d.vert");
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, "Shaders/basic_3d.frag");
	shaderProgram = createShader(vertexShader, fragmentShader);*/

    shaderProgram = createShader("Shaders/basic_3d.vert", "Shaders/basic_3d.frag");
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program for TestMinigame!" << std::endl;
	}

	// Cube Vertices (position and color)
    float cubeVertices[] = {
         -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

         -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

          0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

         -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f
    };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); 

	glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

    // Position camera
	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
}

TestMinigame::~TestMinigame() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void TestMinigame::update(float deltaTime) {
    timeElapsed += deltaTime;

	GLFWwindow* window = glfwGetCurrentContext();
    if (timeElapsed > 5.0f || glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE == GLFW_PRESS)) {
        playerWon = true;
		isGameOver = true;
    }
}

void TestMinigame::render() {
	glUseProgram(shaderProgram);

    // Projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

    // View matrix
	glm::mat4 view = camera.GetViewMatrix();

    // Model matrix (Rotate the cube)
    glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, timeElapsed * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    // Set uniforms
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Render cube
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

bool TestMinigame::checkWinCondition() {
    return playerWon;
}

bool TestMinigame::isFinished() {
    return isGameOver;
}   