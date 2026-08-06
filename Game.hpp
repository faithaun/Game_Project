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

	int getHighScore() const {
		return highScore;
	}
	void updateHighScore(int score);

private: 
	sf::RenderWindow window;
	std::unique_ptr<GameState> currentState;

	int highScore = 0;
	void loadHighScore();
	void saveHighScore();
};
