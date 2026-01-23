#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// This is just an Abstract Base Class for Minigames - a blueprint;
class Minigame {
public:
	virtual ~Minigame() {}

	// Updates game logic (movement, collisions, timer, etc.)
	virtual void update(float deltaTime) = 0;

	// Draws the 3D objects
	virtual void render() = 0;

	// Returns true if the player won
	virtual bool checkWinCondition() = 0;

	// Returns true if the minigame is finished (win or lose)
	virtual bool isFinished() = 0;
};

