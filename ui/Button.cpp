#include "Button.hpp"

Button::Button(const sf::Font& font, const std::string& label, sf::Vector2f position, sf::Vector2f size) {
	shape.setPosition(position);
	shape.setSize(size);
	shape.setFillColor(sf::Color(40,40,40));
	shape.setOutlineColor(sf::Color::White);
	shape.setOutlineThickness(2.0f);

	text.setFont(font);
	text.setString(label);
	text.setCharacterSize(28);
	text.setFillColor(sf::Color::White);

	sf::FloatRect textBounds = text.getLocalBounds();
	text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
	text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}

bool Button::contains(sf::Vector2f point) const { 
	return shape.getGlobalBounds().contains(point);
}

void Button::handleClick(sf::Vector2f mousePos) {
	if (contains(mousePos) && onClick) {
		onClick();
	}
}

void Button::setHovered(bool hovered) {
	isHovered = hovered;
	shape.setFillColor(hovered ? sf::Color(70, 70, 70) : sf::Color(40, 40, 40));
}

void Button::draw(sf::RenderWindow& window) const {
	window.draw(shape);
	window.draw(text);
}
