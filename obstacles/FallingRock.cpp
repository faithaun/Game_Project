#include "FallingRock.hpp"
#include "FallMovement.hpp"
#include "../Player.hpp"

FallingRock::FallingRock(sf::Vector2f position, float fallSpeed) 
	: Obstacle(position, sf::Vector2f(40.f, 40.f), sf::Color(120, 100, 90), 
		std::make_unique<FallMovement>(fallSpeed), "resources/falling_rock.png") {
}

bool FallingRock::onHitFromBelow(Player& player) {
	return !player.isShieldActive();
}

bool FallingRock::onLandedOnTop(Player& player) {
	return !player.isShieldActive();
}

bool FallingRock::isShootable() const {
	return false;
}
