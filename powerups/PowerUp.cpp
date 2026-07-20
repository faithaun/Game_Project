#include "PowerUp.hpp"

PowerUp::PowerUp(sf::Vector2f position) {
    shape.setPosition(position);
}

sf::FloatRect PowerUp::getBounds() const {
    return shape.getGlobalBounds();
}

void PowerUp::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

