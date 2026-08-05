#include "Bullet.hpp"

Bullet::Bullet() {
	shape.setSize({10.f, 16.f});
	shape.setFillColor(sf::Color(230, 220, 60)); //banana yellow
}

void Bullet::activate(sf::Vector2f position) {
	shape.setPosition(position);
	active = true;
	movingDown = false;  
}

void Bullet::update(float deltaTime) {
	if (!active) {
		return;
	} 
	shape.move(0.f, movingDown ? SPEED * deltaTime : -SPEED * deltaTime);
		//negative Y = upward, Y = downward
}

void Bullet::bounce() {
	movingDown = true;
}

void Bullet::deactivate() {
	active = false; 
}

bool Bullet::isActive() const {
	return active;
}

sf::FloatRect Bullet::getBounds() const {
	return shape.getGlobalBounds();
}

void Bullet::draw(sf::RenderWindow& window) const {
	if (active) {
		window.draw(shape);
	}
}
