#include "Shield.hpp"
#include "../Player.hpp"
#include <iostream> 

Shield::Shield(sf::Vector2f position) : PowerUp(position) {
	//shape stays invisible - only use for collision detection
	shape.setSize({45.f, 45.f});
	shape.setPosition(position);
	
	if (!texture.loadFromFile("resources/shield.png")) {
		std::cerr << "Warning: could not load resources shield" << std::endl;
	}

	sprite.setTexture(texture);
	sf::Vector2u texSize = texture.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		sprite.setScale(45.f / texSize.x, 45.f / texSize.y);
	}
	sprite.setPosition(position);
}


void Shield::apply(Player& player) {
	player.activateShield(SHIELD_DURATION);
}

void Shield::draw(sf::RenderWindow& window) const {
	window.draw(sprite);   //draw actual shield image
}
