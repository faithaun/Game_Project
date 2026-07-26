// swtiching MainMenuState -> PlayState works
//collision dectection lives  


#pragma once
#include "GameState.hpp"
#include "../Player.hpp"
#include "../powerups/PowerUp.hpp"
#include "../obstacles/Obstacle.hpp"
#include <vector>
#include <memory>

class Game;

//playform simple dont need own class or file
struct Platform {
	sf::RectangleShape shape;
	std::unique_ptr<PowerUp>attachedPowerUp;	 //nullptr if this platform has none
	std::vector<std::unique_ptr<Obstacle>> obstacles;
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
	std::vector<std::unique_ptr<Obstacle>>obstacles;	

	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	sf::View gameView;  //scrolling camera
	float cameraCenterY;  // current vertical center of camera
	float bestHeightY;    //highest Y the player has ever reach for score
	int score;
	bool isGameOver;
	bool isFallingFromBirdHit;
	int birdsSpawnedThisBlock;
	int currentScoreBlock;
	int springUnlockScore;
	int maxSpringThisGame;
	int springSpawnedSoFar;
	float obstacleSpawnTimer;
	bool cameraFollowingDown;
	float fallStartBottomY;

	sf::Font font;
	bool fontLoaded;
	sf::Text scoreText;
	sf::Text gameOverText;
	sf::Text restartText;

	void spawnPlatforms();
	void recyclePlatforms();
	void handleCollisions(float previousPlayerBottom); 

	void spawnObstacles(float deltaTime);
	void recycleObstacles();
	void checkObstacles(float previousPlayerBottom);

	void updateCamera();
	void checkPowerUps();
	void updateScore();
	void restartGame();
};
