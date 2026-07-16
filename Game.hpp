//  currently active GameState

#pragma once
#include <SFML/Graphics.hpp>
#include <memory> 
#include "state/GameState.hpp"

class Game {
public:  
	Game();
	void run();
	
	void changeState(std::unique_ptr<GameState> newState);
	void requestQuit() {window.close(); }

private: 
	sf::RenderWindow window;
	std::unique_ptr<GameState> currentState;
};
