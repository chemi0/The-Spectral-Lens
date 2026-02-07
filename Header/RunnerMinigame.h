#pragma once
#include "Minigame.h"
#include "Camera.h"
#include "Model.h"
#include "Util.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum ObstacleType {
	GROUND, // Jump Over
	FLYING_LOW // Duck Under

};

// Obstacle structure
struct Obstacle {
	glm::vec3 position;
	bool active;
	ObstacleType type;
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

	// Horizontal Movement (Lane Switching)
	int currentLane;
	float playerX, targetX;
	bool wasRightPressed, wasLeftPressed = false;

	// Vertical Movement (Jumping)
	float playerY; // Current "height" of the player
	float verticalVelocity;
	bool isJumping;
	const float GRAVITY = -35.0f;
	const float JUMP_FORCE = 12.0f;

	// Vertical Movement (Ducking)
	float playerScaleY, targetScaleY; // Current target scale for ducking
	const float DUCK_SPEED = 10.0f; // How fast the player ducks/stands up

	// World Generation
	std::vector<Obstacle> obstacles;
	float spawnTimer;
	float gameSpeed;
	float survivalTimer;
	bool isGameOver, playerWon;
	float groundOffset;  // Ground scroll offset for movement illusion

	// HUD resources
	unsigned int hudShaderProgram;
	unsigned int hudVAO, hudVBO;
	unsigned int fontTexture;

	// Helper functions
	void spawnObstacle();
	void resetGame();
	void renderCube(glm::mat4 model, glm::vec3 color);
	void initializeHUD();
	void renderTimerHUD();
	void renderObstacle(const Obstacle& obs);

	// Models
	Model playerModel;
	Model obstacleModel;
	Model groundModel;  // Textured ground platform

};