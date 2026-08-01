#include "BeeNest.hpp"
#include "StaticMovement.hpp"
#include "../Player.hpp"

BeeNest::BeeNest(sf::Vector2f position) : Obstacle(position, sf::Vector2f(75.f, 75.f), sf::Color(200, 150, 60),
	std::make_unique<StaticMovement>(), "resources/bee_nest.png") {
}

bool BeeNest::onHitFromBelow(Player& player) {
	return !player.isShieldActive();
}

bool BeeNest::onLandedOnTop(Player& player) {
	return !player.isShieldActive();
}

void BeeNest::onUpdate(float deltaTime) {
	if (readyToSpawn) return;
	spawnTimer -= deltaTime;
	if (spawnTimer <= 0.f) {
		readyToSpawn = true;
	}
}
