#pragma once
#include "Minigame.h"
#include "Camera.h"
#include "Util.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Obstacle structure
struct Obstacle {
	glm::vec3 position;
	bool active;
};

class RunnerMinigame : public Minigame {
public:
	RunnerMinigame(int width, int height);
	~RunnerMinigame();

	void update(float deltaTime) override;
	void render() override;
	bool checkWinCondition() override;
	bool isFinished() override;

private:
	unsigned int shaderProgram;
	unsigned int VAO, VBO;

	// Camera
	Camera camera;
	int screenWidth, screenHeight;

	// Game logic
	float timeElapsed;
	bool isGameOver;
	bool playerWon;
	float survivalTimer; // Player needs to survive for this duration to win

	// Player State
	int currentLane; // 0 = Left; 1 = Center; 2 = Right
	float playerX; // Current visual X
	float targetX; // Where the player wants to go

	// World Generation
	std::vector<Obstacle> obstacles;
	float spawnTimer;
	float gameSpeed;

	// Helper functions
	void spawnObstacle();
	void resetGame();
	void renderCube(glm::mat4 model, glm::vec3 color);

	bool wasLeftPressed = false;
	bool wasRightPressed = false;
};