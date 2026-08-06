// Mainm menu: title + play button
#pragma once
#include "GameState.hpp"
#include "../ui/Button.hpp"
#include <vector>

class Game;

class MainMenuState : public GameState {
public: 
	explicit MainMenuState(Game& game);
	
	void onEnter() override;
	void handleEvent(const sf::Event& event) override;
	void update(float deltaTime) override;
	void render(sf::RenderWindow& window) override;

private:
	Game& game;
	sf::Font font;

	sf::Sprite backgroundSprite;
	sf::Sprite mascotSprite;
	sf::Sprite titleSprite;
	std::vector<Button> buttons;

	void setupBackground();
	void setupTitle();
	void setupButtons();
	void setupMascot();

};
