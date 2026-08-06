#include "Obstacle.hpp"
#include "../ResourceManager.hpp"

Obstacle::Obstacle(sf::Vector2f position, sf::Vector2f size, sf::Color color, std::unique_ptr<MovementStrategy> strategy, const std::string& texturePath)
	: movementStrategy(std::move(strategy)) {
	shape.setPosition(position);
	shape.setSize(size);
	shape.setFillColor(color);

	if (!texturePath.empty()) {
		sf::Texture& texture = ResourceManager::getInstance().getTexture(texturePath);
		shape.setTexture(&texture);
		shape.setFillColor(sf::Color::White);   //white so png real color
	}
}

void Obstacle::update(float deltaTime) {
	movementStrategy->move(shape, deltaTime);
	onUpdate(deltaTime);
}

sf::FloatRect Obstacle::getBounds() const {
	return shape.getGlobalBounds();
}

void Obstacle::draw(sf::RenderWindow& window) const {
	window.draw(shape);
}

