#pragma once
#include <GL/glew.h>

enum class MovementType {
	Static, // No movement, for the entities that are just "hidden" in the scene
	HorizontalLoop, // Flying in a loop horizontally (wrap around)
	Bounce, // Cosine up/down jumping movement
	ReverseParabolicCurve,
};

struct Entity {
	float posX;
	float posY;
	float width;
	float height;
	float speed;
	unsigned int texture;
	MovementType movement;
	bool found; // discovery flag

	// For the reset functionality 
	float initialPosX; 
	float initialPosY;

	bool facingRight;

	Entity() :
		posX(0.0f), posY(0.0f), width(1.0f), height(1.0f), speed(0.0f), texture(0), movement(MovementType::Static), found(false), initialPosX(0.0f), initialPosY(0.0f), facingRight(false) {}

};

