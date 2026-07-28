#include "Banana.hpp"
#include "../Player.hpp"
#include <iostream>

Banana::Banana(sf::Vector2f position) : PowerUp(position) {
	//shape stays invisible- only used for collision detection (get(bounds())
	shape.setSize({45.f, 45.f});
	shape.setPosition(position);

	if (!texture.loadFromFile("resources/banana.png")) {
		std::cerr << "Warning: could not load resources/banana.png" << std::endl;
	}
	sprite.setTexture(texture);
	sf::Vector2u texSize = texture.getSize();
	
	if (texSize.x > 0 && texSize.y > 0) {
		sprite.setScale(45.f / texSize.x, 45.f / texSize.y);
	} 
	sprite.setPosition(position);
}

	
void Banana::apply(Player& player) {
	player.addAmmo(5);  //5 ammo = 1 banana collected
	player.recordBananaCollected(); //lifetime stat - never decreases when shooting
}

void Banana::draw(sf::RenderWindow& window) const {
	window.draw(sprite); //draw actual banana image
}
