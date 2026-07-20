#include "Spring.hpp"
#include "../Player.hpp"

Spring::Spring(sf::Vector2f position) : PowerUp(position) {
    shape.setSize({20.f, 20.f});
    shape.setFillColor(sf::Color::Yellow);
    shape.setOutlineColor(sf::Color(150, 120, 0));
    shape.setOutlineThickness(2.f);
}

void Spring::apply(Player& player) {
    player.bigJump();
}


