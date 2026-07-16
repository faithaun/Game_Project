#include "Game.hpp"
#include "state/MainMenuState.hpp"

Game::Game() : window(sf::VideoMode(540, 700), "Jungle Jumper") {
	window.setFramerateLimit(60);
	changeState(std::make_unique<MainMenuState>(*this));
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
