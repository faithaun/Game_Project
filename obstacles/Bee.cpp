#include "Bee.hpp"
#include "AngledSwarmMovement.hpp"
#include "../Player.hpp"

Bee::Bee(sf::Vector2f position, float angleRadians, float speed) 
	: Obstacle(position, sf::Vector2f(40.f, 40.f), sf::Color(230, 190, 0),
	std::make_unique<AngledSwarmMovement>(angleRadians, speed, 45.f, 5.f), "resources/bee.png") {
}

bool Bee::onHitFromBelow(Player& player) {
	return !player.isShieldActive();
}

bool Bee::onLandedOnTop(Player& player) {
	return !player.isShieldActive();
}
