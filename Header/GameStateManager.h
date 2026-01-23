#pragma once
#include "Minigame.h"
#include "Renderer.h"
#include "Entity.h"
#include "InputManager.h"

enum class GameState {
	LENS_2D, // 2D exploration mode
	MINIGAME_3D // 3D minigame mode
};

class GameStateManager {
public:
	// Singleton access [one instance of the class]
	static GameStateManager& getInstance() {
		static GameStateManager instance;
		return instance;
	}

	void update(float deltaTime);
	void render(const InputManager& input);

	// Called after clicking an entity in 2D mode: the minigame movement type matches the entity's
	void startMinigame(MovementType type, Entity* entity);

	// Helper function to return to 2D
	void returnTo2D(bool playerWon);

	void setRenderer(Renderer* r) { mainRenderer = r; }
	GameState getState() const { return currentState; }

private:
	GameStateManager() : currentState(GameState::LENS_2D), currentMinigame(nullptr), mainRenderer(nullptr), activeEntity(nullptr) {}

	// Prevent copying (Singleton pattern)
	GameStateManager(const GameStateManager&) = delete;
	void operator = (const GameStateManager&) = delete;

	GameState currentState;
	Minigame* currentMinigame;

	Renderer* mainRenderer; // Pointer to the main renderer
	Entity* activeEntity; // The entity that the player is playing the minigame for

};