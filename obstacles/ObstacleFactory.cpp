#include "ObstacleFactory.hpp"
#include "Bird.hpp"
#include "FallingRock.hpp"
#include "Bee.hpp"
#include "BeeNest.hpp"

std::unique_ptr<Obstacle> ObstacleFactory::createBird(sf::Vector2f position) {
	return std::make_unique<Bird>(position);
}

std::unique_ptr<Obstacle> ObstacleFactory::createFallingRock(sf::Vector2f position, float fallSpeed) {
	return std::make_unique<FallingRock>(position, fallSpeed);
}

std::unique_ptr<Obstacle> ObstacleFactory::createBee(sf::Vector2f position, float angleRadians, float speed) {
	return std::make_unique<Bee>(position, angleRadians, speed);
}

std::unique_ptr<Obstacle> ObstacleFactory::createBeeNest(sf::Vector2f position) {
	return std::make_unique<BeeNest>(position);
}
