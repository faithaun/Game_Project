#include "Button.hpp"

namespace {
	const sf::Color kWoodFill(140, 90, 43);
	const sf::Color kWoodFillHover(160, 108, 58);
	const sf::Color kWoodOutline(70, 40, 20);
}


Button::Button(const sf::Font& font, const std::string& label, sf::Vector2f position, sf::Vector2f size) : position(position), size(size) {	
	
	body.setSize(size);
	body.setPosition(position);
	applyColor(kWoodFill);
	body.setOutlineColor(kWoodOutline);
	body.setOutlineThickness(3.f);

	text.setFont(font);
	text.setString(label);
	text.setCharacterSize(static_cast<unsigned int>(size.y * 0.42f));
	text.setStyle(sf::Text::Bold);
	text.setFillColor(sf::Color(255, 248, 220)); //warm cream

	sf::FloatRect textBounds = text.getLocalBounds();
	text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
	text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}
void Button::applyColor(sf::Color fill) {
	body.setFillColor(fill);
}

bool Button::contains(sf::Vector2f point) const { 
	sf::FloatRect fullBounds(position.x, position.y, size.x, size.y);
	return fullBounds.contains(point);
}

void Button::handleClick(sf::Vector2f mousePos) {
	if (contains(mousePos) && onClick) {
		onClick();
	}
}

void Button::setHovered(bool hovered) {
	isHovered = hovered;
	applyColor(hovered ? kWoodFillHover : kWoodFill);
}

void Button::draw(sf::RenderWindow& window) const {
	window.draw(body);
	window.draw(text);
}
