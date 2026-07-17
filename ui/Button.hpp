// clickable button: shape + label + callback

#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button {
public: 
	Button(const sf::Font& font, const std::string& label, sf::Vector2f position, sf::Vector2f size);
	void setOnClick(std::function<void()>callback) {onClick = callback; }

	bool contains(sf::Vector2f point) const;
	void handleClick(sf::Vector2f mousePos);
	void setHovered(bool hovered);
	void draw(sf::RenderWindow& window) const;

private:
	sf::Vector2f position;
	sf::Vector2f size;

// rounded rectangle 
	sf::RectangleShape body;
	sf::Text text;

	std::function<void()>onClick;
	bool isHovered = false;
	
	void applyColor(sf::Color fill);
};
