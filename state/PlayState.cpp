#include "PlayState.hpp"
#include "../Game.hpp"
#include <iostream>

PlayState::PlayState(Game& game) : game(game) {
} 

void PlayState::onEnter() {
	std::cerr << "Entered Play State" << std::endl;
}

void PlayState::handleEvent(const sf::Event& event) {
//handle 
}

void PlayState::update(float deltaTIme) {
//update
}

void PlayState::render(sf::RenderWindow& window) {
	window.clear(sf::Color(50, 150, 50));
}
