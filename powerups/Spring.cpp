#include "Spring.hpp"
#include "../Player.hpp"
#include <iostream>

Spring::Spring(sf::Vector2f position) : PowerUp(position) {
	shape.setSize({30.f, 30.f});

	sf::Texture& texture = getTexture();
	shape.setTexture(&texture);
    	shape.setFillColor(sf::Color::White);   //white so png real colors, no tint 
}

void Spring::apply(Player& player) {
    	player.bigJump();
}

sf::Texture& Spring::getTexture() {
	static sf::Texture texture;
	static bool loaded = false;
	if (!loaded) {
		if (!texture.loadFromFile("resources/spring.png")) {
			std::cerr << "Warning: could not load spring" << std::endl;
		} 
		loaded = true;
	}
	return texture;
}
