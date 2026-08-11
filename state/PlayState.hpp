// swtiching MainMenuState -> PlayState works
//collision dectection lives  


#pragma once
#include "../ui/Button.hpp"
#include "GameState.hpp"
#include "../platform/Platform.hpp"
#include "../Player.hpp"
#include "../powerups/PowerUp.hpp"
#include "../obstacles/Obstacle.hpp"
#include "../bullet/BulletPool.hpp"
#include <vector>
#include <memory>

class Game;

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
	BulletPool bulletPool;

	sf::Sprite backgroundSprite;
	sf::Sprite groundSprite;

	sf::View gameView;  //scrolling camera
	float cameraCenterY;  // current vertical center of camera
	float bestHeightY;    //highest Y the player has ever reach for score
	int score;

	float highScoreLineY;
	bool highScoreSavedThisRun;

	bool isGameOver;
	bool isFallingFromBirdHit;
	int birdsSpawnedThisBlock;
	std::vector<std::pair<float, float>> recentlyClearedSpots;  	
	float lastObstacleSpawnY = -100000.f; //most recent obstacle spawn event; start far away so first spawn is allowed
	
	int currentScoreBlock;
	int springUnlockScore;
	int maxSpringThisGame;
	int springSpawnedSoFar;
	float obstacleSpawnTimer;
	bool cameraFollowingDown;
	float fallStartBottomY;
	int nextRockWaveScore = 3000;
	bool rockWarningActive = false;
	float rockWarningTimer = 0.f;
	float beeNestSpawnTimer = 0.f;

	sf::Font font;
	bool fontLoaded;
	sf::Text scoreText;
	sf::Text gameOverText;
	std::vector<Button> gameOverButtons;

	bool isPaused = false;
	sf::FloatRect pauseIconBounds;
	sf::Text pausedText;
	sf::Sprite resumeSprite;
	sf::Text resumeText;
	sf::Text finalHighScoreText;
	sf::Text finalScoreText;

	sf::Sprite bananaIconSprite;
	sf::Text bananaCountText;
	sf::Sprite ammoIconSprite;
	sf::Text ammoText;

	void spawnPlatforms();
	void recyclePlatforms();
	void handleCollisions(float previousPlayerBottom); 

	void spawnObstacles(float deltaTime);
	void recycleObstacles();
	void checkObstacles(float previousPlayerBottom);
	void checkBulletHits();
	void spawnRockWave();
	void spawnBeeNest(float deltaTime);

	void updateCamera();
	void checkPowerUps();
	void updateScore();
	void restartGame();
};
