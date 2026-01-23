#pragma once
#include "Minigame.h"
#include "Camera.h"
#include "Util.h" // For loading shaders
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class TestMinigame : public Minigame {
public:
	TestMinigame(int width, int height);
	~TestMinigame();

	void update(float deltaTime) override;
	void render() override;
	bool checkWinCondition() override;
	bool isFinished() override;

private:
	unsigned int shaderProgram;
	unsigned int VBO, VAO;

	Camera camera;
	int screenWidth, screenHeight;

	bool isGameOver;
	bool playerWon;
	float timeElapsed;
};