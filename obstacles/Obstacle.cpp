#include "Obstacle.hpp"
#include <iostream>

Obstacle::Obstacle(sf::Vector2f position, sf::Vector2f size, sf::Color color, std::unique_ptr<MovementStrategy> strategy, const std::string& texturePath)
	: movementStrategy(std::move(strategy)) {
	shape.setPosition(position);
	shape.setSize(size);
	shape.setFillColor(color);

	if (!texturePath.empty()) {
		if (texture.loadFromFile(texturePath)) {
			shape.setTexture(&texture);
			shape.setFillColor(sf::Color::White);    //white so png real colors show through
		} else { 
			std::cerr << "Warning: could not load" << texturePath << std::endl;
		}
	}
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

