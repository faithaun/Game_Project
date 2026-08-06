#include "Game.hpp"
#include "state/MainMenuState.hpp"
#include <fstream>

Game::Game() : window(sf::VideoMode(540, 700), "Jungle Jumper"), highScore(0) {
	window.setFramerateLimit(60);
	loadHighScore();
	changeState(std::make_unique<MainMenuState>(*this));
}

void Game::loadHighScore() {
	std::ifstream in("highscore.txt");
	if (in) {
		in >> highScore;
	} 
	//file doesnt exist yet
}

void Game::updateHighScore(int score) {
	if (score > highScore) {
		highScore = score;
		std::ofstream out ("highscore.txt");
		if (out) {
			out << highScore;
		}
	}
}

void Game::changeState(std::unique_ptr<GameState> newState) {
	if (currentState) {
		currentState->onExit();
	}
	currentState = std::move(newState);
	currentState->onEnter();
}

void Game::run() {
	sf::Clock clock;
	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} 
			currentState->handleEvent(event);
		} 
	
		currentState->update(deltaTime);
		currentState->render(window);
		window.display();
	} 
}
