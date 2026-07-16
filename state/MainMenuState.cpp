#include "MainMenuState.hpp"
#include "../Game.hpp"
#include <iostream>

MainMenuState::MainMenuState(Game& game) : game(game) {
	if (!font.loadFromFile("resources/LuckiestGuy.ttf"))  {
		std::cerr << "Failed to load resources/LuckiestGuy.ttf" << std::endl;
	}

	setupBackground();
	setupTitle();
	setupButtons();

}

void MainMenuState::setupBackground() {
	if (!backgroundTexture.loadFromFile("resources/MainMenu.png")) {
		std::cerr << "Failed to load resources/MainMenu.png" << std::endl;
	}
	
	backgroundSprite.setTexture(backgroundTexture);

	//scale the image
	sf::Vector2u textureSize = backgroundTexture.getSize();
	backgroundSprite.setScale( 
		540.f / textureSize.x, 700.f / textureSize.y 
	);
} 
  
void MainMenuState::setupTitle() {
	titleLine1.setFont(font);
	titleLine1.setString("Jungle");
	titleLine1.setCharacterSize(90);
	titleLine1.setFillColor(sf::Color::White);
	titleLine1.setOutlineColor(sf::Color::Black);
	titleLine1.setOutlineThickness(4.f);

	sf::FloatRect bounds1 = titleLine1.getLocalBounds();
	titleLine1.setOrigin(bounds1.left + bounds1.width / 2.0f, 0.f);
	titleLine1.setPosition(270.f, 30.f);

	titleLine2.setFont(font);
	titleLine2.setString("Jumper");
	titleLine2.setCharacterSize(90);
	titleLine2.setFillColor(sf::Color::White);
	titleLine2.setOutlineColor(sf::Color::Black);
	titleLine2.setOutlineThickness(4.f);

	sf::FloatRect bounds2 = titleLine2.getLocalBounds();
	titleLine2.setOrigin(bounds2.left + bounds2.width / 2.0f, 0.f);
	titleLine2.setPosition(270.f, 130.f);
} 


void MainMenuState::setupButtons() {
	//Play button for now
	buttons.emplace_back(font, "Play", sf::Vector2f(190.f, 300.f), sf::Vector2f(160.f, 55.f));
	buttons.back().setOnClick([this]() {
		std::cerr << "Play pressed -> TODO: switch to PlayState" << std::endl;
	});
}

void MainMenuState::onEnter() {
	std::cerr << "Entered Main Menu" << std::endl;
}

void MainMenuState::handleEvent(const sf::Event& event) {
	if (event.type == sf::Event::MouseMoved) {
		sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
		for (auto& button : buttons) {
			button.setHovered(button.contains(mousePos));
		}
 	} 

	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
		for (auto& button : buttons) {
			button.handleClick(mousePos);
		} 
	} 
} 


void MainMenuState::update (float deltaTime) {
//nothing to animate yet :)  
} 

void MainMenuState::render(sf::RenderWindow& window) {
	window.clear();
	window.draw(backgroundSprite);
	window.draw(titleLine1);
	window.draw(titleLine2);
	for (const auto& button : buttons) {
		button.draw(window);
	} 
}
