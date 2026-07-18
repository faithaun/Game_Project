// swtiching MainMenuState -> PlayState works
//collision dectection lives  


#pragma once
#include "GameState.hpp"
#include "../Player.hpp"
#include <vector>

class Game;

//playform simple dont need own class or file
struct Platform {
	sf::RectangleShape shape;
};

class PlayState : public GameState {
public:
	explicit PlayState(Game& game);
	
	void onEnter() override;
	void handleEvent(const sf::Event& event) override;
	void update(float deltaTime) override;
	void render(sf::RenderWindow& window) override;

private:
	Game& game;
	Player player;
	std::vector<Platform> platforms;
	
	sf::View gameView;  //scrolling camera
	float cameraCenterY;  // current vertical center of camera
	float bestHeightY;    //highest Y the player has ever reach for score
	int score;
	bool isGameOver;

	sf::Font font;
	bool fontLoaded;
	sf::Text scoreText;
	sf::Text gameOverText;
	sf::Text restartText;

	void spawnPlatforms();
	void recyclePlatforms();
	void handleCollisions(float previousPlayerBottom);	
	void updateCamera();
	void updateScore();
	void restartGame();
};
