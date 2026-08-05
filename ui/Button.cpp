#include "Button.hpp"
#include <iostream>

namespace {
	const sf::Color kNormalTint(255, 255, 255);
	const sf::Color kHoverTint(200, 200, 200);
}


Button::Button(const sf::Font& font, const std::string& label, sf::Vector2f position, sf::Vector2f size, const 
	std::string& texturePath) : position(position), size(size) {	
	
	if (!bodyTexture.loadFromFile(texturePath)) {
		std::cerr << "Failed to load " << texturePath << std::endl;
	}
	body.setTexture(bodyTexture);
	sf::Vector2u texSize = bodyTexture.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		body.setScale(size.x / texSize.x, size.y / texSize.y);
	}

	body.setPosition(position);
	applyColor(kNormalTint);

	text.setFont(font);
	text.setString(label);
	text.setFillColor(sf::Color(255, 250, 230)); //warm cream

	//for setting text font
	unsigned int charSize = static_cast<unsigned int>(size.y * 0.34f);
	text.setCharacterSize(charSize);

	float maxTextWidth = size.x * 0.28f;  //leave rooom after icon
	while (text.getLocalBounds().width > maxTextWidth && charSize > 10) {
		charSize -= 2;
		text.setCharacterSize(charSize);
	}


	sf::FloatRect textBounds = text.getLocalBounds();
	text.setOrigin(textBounds.left, textBounds.top + textBounds.height / 2.0f);
	text.setPosition(position.x + size.x * 0.42f, position.y + size.y / 2.0f);
}
void Button::applyColor(sf::Color fill) {
	body.setColor(fill);
}

void Button::setHovered(bool hovered) {
	isHovered = hovered;
	applyColor(hovered ? kHoverTint : kNormalTint);
}

bool Button::contains(sf::Vector2f point) const { 
	return body.getGlobalBounds().contains(point);
}

void Button::handleClick(sf::Vector2f mousePos) {
	if (contains(mousePos) && onClick) {
		onClick();
	}
}

void Button::draw(sf::RenderWindow& window) const {
	window.draw(body);
	window.draw(text);
}
