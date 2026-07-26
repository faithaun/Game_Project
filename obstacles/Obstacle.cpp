#include "Obstacle.hpp"

Obstacle::Obstacle(sf::Vector2f position, sf::Vector2f size, sf::Color color, std::unique_ptr<MovementStrategy> strategy)
	: movementStrategy(std::move(strategy)) {
	shape.setPosition(position);
	shape.setSize(size);
	shape.setFillColor(color);
} 

void Obstacle::update(float deltaTime) {
	movementStrategy->move(shape, deltaTime);
}

sf::FloatRect Obstacle::getBounds() const {
	return shape.getGlobalBounds();
}

void Obstacle::draw(sf::RenderWindow& window) const {
	window.draw(shape);
}

