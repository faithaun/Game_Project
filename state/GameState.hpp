#pragma once
#include <SFML/Graphics.hpp>

class GameState {
public: 
	virtual ~GameState() = default;

	virtual void onEnter() {}
	virtual void onExit() {}
	
	virtual void handleEvent(const sf::Event& event) = 0;
	virtual void update(float deltaTime) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
};
