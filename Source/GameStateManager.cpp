#include "../Header/GameStateManager.h"
#include "../Header/TestMinigame.h"
#include <iostream>

void GameStateManager::update(float deltaTime) {
	if (currentState == GameState::LENS_2D) {
		// I doubt anything needs to be driven here, since Main.cpp usually drives the 2D update, but just in case I decide to move the logic here
	}
	else if (currentState == GameState::MINIGAME_3D) {
		if (currentMinigame) {
			currentMinigame->update(deltaTime);

			if (currentMinigame->isFinished()) {
				bool won = currentMinigame->checkWinCondition();
				returnTo2D(won);
			}
		}
	}
}

void GameStateManager::render(const InputManager& input) {
	if (currentState == GameState::LENS_2D) {
		if (mainRenderer) {

			// Disable Depth Test for 2D (crucial, otherwise nothing will render)
			glDisable(GL_DEPTH_TEST);

			// Draw the 2D scene
			mainRenderer->render(input);
		}
	}
	else if (currentState == GameState::MINIGAME_3D) {
		
		// Enable Depth Test for 3D
		glEnable(GL_DEPTH_TEST);
		
		// Clear buffers (Color + Depth)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (currentMinigame) {
			currentMinigame->render();
		}
	}
}

void GameStateManager::startMinigame(MovementType type, Entity* entity) {
	std::cout << "Starting Minigame for Entity type: " << (int)type << std::endl;

	activeEntity = entity;
	currentState = GameState::MINIGAME_3D;

	// The logic for which movement type goes for which minigame will go here (when I actually define all of the minigames)
	currentMinigame = new TestMinigame(1920, 1080);

	// Temp placeholder:
	std::cout << "3D Test Scene Initialized. Press SPACE to WIN." << std::endl;
	// returnTo2D(true); // Uncomment later to test returning to 2D
}

void GameStateManager::returnTo2D(bool playerWon) {
	std::cout << "Returning to 2D. Result: " << (playerWon ? "Won" : "Lost") << std::endl;

	if (playerWon && activeEntity) {
		activeEntity->found = true;
	}

	// Memory cleanup
	if (currentMinigame) {
		delete currentMinigame;
		currentMinigame = nullptr;
	}

	currentState = GameState::LENS_2D;
	activeEntity = nullptr;
}

