#include "Bird.hpp"
#include "StaticMovement.hpp"
#include "../Player.hpp"

Bird::Bird(sf::Vector2f position) : Obstacle(position, sf::Vector2f(40.f, 30.f), sf::Color(70, 70, 70),
	std::make_unique<StaticMovement>()) {
}

bool Bird::onHitFromBelow(Player& player) {
	//dizzy - always fatar, no exception
	return true;
}



void Bird::onLandedOnTop(Player& player) {
	//normal jump - weaker than spring big jump()
	player.jump();
}
