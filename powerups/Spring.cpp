#include "Spring.hpp"
#include "../Player.hpp"
#include "../ResourceManager.hpp"

Spring::Spring(sf::Vector2f position) : PowerUp(position) {
	shape.setSize({30.f, 30.f});

	sf::Texture& texture = ResourceManager::getInstance().getTexture("resources/spring.png");
	shape.setTexture(&texture);
	    	shape.setFillColor(sf::Color::White);   //white so png real colors, no tint 
}

void Spring::apply(Player& player) {
    	player.bigJump();
}

