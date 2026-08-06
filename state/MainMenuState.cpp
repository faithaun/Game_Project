#include "MainMenuState.hpp"
#include "../Game.hpp"
#include "PlayState.hpp"
#include <iostream>
#include "../ResourceManager.hpp"

MainMenuState::MainMenuState(Game& game) : game(game) {
	if (!font.loadFromFile("resources/LuckiestGuy.ttf"))  {
		std::cerr << "Failed to load resources/LuckiestGuy.ttf" << std::endl;
	}

	setupBackground();
	setupTitle();
	setupButtons();
	setupMascot();

}

void MainMenuState::setupBackground() {
	sf::Texture& backgroundTexture = ResourceManager::getInstance().getTexture("resources/ground_background.png");
	
	backgroundSprite.setTexture(backgroundTexture);

	//scale the image
	sf::Vector2u textureSize = backgroundTexture.getSize();
	backgroundSprite.setScale( 
		540.f / textureSize.x, 700.f / textureSize.y 
	);
} 
  
void MainMenuState::setupTitle() {
	sf::Texture& titleTexture = ResourceManager::getInstance().getTexture("resources/title_logo.png");
	titleSprite.setTexture(titleTexture);

	sf::Vector2u texSize = titleTexture.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		float targetWidth = 340.f;
		float scale = targetWidth / texSize.x;
		titleSprite.setScale(scale, scale);
		titleSprite.setOrigin(texSize.x / 2.f, 0.f);
	
	}
	titleSprite.setPosition(540.f / 2.f, 40.f);
} 


void MainMenuState::setupButtons() {
	buttons.reserve(3);    //revents vector from reallocating

	sf::Vector2f buttonSize(400.f, 100.f); //same size for every button
	float buttonX = (540.f - buttonSize.x) / 2.f;   //center

	buttons.emplace_back(font, "Play", sf::Vector2f(buttonX, 280.f), buttonSize, "resources/button_play.png");
	buttons.back().setOnClick([this]() {
		std::cerr << "Play pressed -> Switch to PlayState" << std::endl;
		game.changeState(std::make_unique<PlayState>(game));
	});


//Setings button
	buttons.emplace_back(font, "Settings", sf::Vector2f(buttonX, 390.f), buttonSize, "resources/button_settings.png");
	buttons.back().setOnClick([this]() {
		std::cerr << "Settings pressed - TODO" << std::endl;
	});

//Exit button 
	buttons.emplace_back(font, "Exit", sf::Vector2f(buttonX, 485.f), buttonSize, "resources/button_exit.png");
	buttons.back().setOnClick([this]() { 
		game.requestQuit();
	});
}

void MainMenuState::setupMascot() {
	sf::Texture& mascotTexture = ResourceManager::getInstance().getTexture("resources/monkey.png");
	mascotSprite.setTexture(mascotTexture);
	
	sf::Vector2u texSize = mascotTexture.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		float targetHeight = 120.f;
		float scale = targetHeight / texSize.y;
		mascotSprite.setScale(scale, scale);
	}
	mascotSprite.setPosition(20.f, 700.f - 130.f); //bottom left corner 
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
	window.draw(titleSprite);
	for (const auto& button : buttons) {
		button.draw(window);
	} 
	window.draw(mascotSprite);
	window.display();
}
