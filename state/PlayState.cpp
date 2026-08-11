#include "PlayState.hpp"
#include "../Game.hpp" 
#include "../powerups/Spring.hpp"
#include "../powerups/Banana.hpp"
#include "../powerups/Shield.hpp"
#include "../obstacles/Bird.hpp"
#include "../obstacles/FallingRock.hpp"
#include "../obstacles/BeeNest.hpp"
#include "../obstacles/Bee.hpp"
#include "../platform/MovingBehavior.hpp"
#include "../platform/BreakingBehavior.hpp"
#include "../ResourceManager.hpp"
#include "../obstacles/ObstacleFactory.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>

namespace {
	constexpr float WINDOW_WIDTH = 540.f;
	constexpr float WINDOW_HEIGHT = 700.f;

	constexpr float PLATFORM_WIDTH = 80.f;
	constexpr float PLATFORM_HEIGHT = 60.f;
	constexpr int PLATFORM_COUNT = 20;                // 12 min platforms, per spec

	constexpr float EASY_MIN_SPACING = 35.f;
	constexpr float EASY_MAX_SPACING = 55.f;
	constexpr float HARD_MIN_SPACING = 50.f;
	constexpr float HARD_MAX_SPACING = 80.f;
	constexpr int DIFFICULTY_SCORE = 150;

	constexpr int SPRING_UNLOCK_SCORE = 500;
	constexpr int BANANA_UNLOCK_SCORE = 200;   //banana start appearing this score
	constexpr int SHIELD_UNLOCK_SCORE = 400;
	constexpr int MOVING_PLATFORM_UNLOCK_SCORE = 400;
	constexpr int BREAKING_PLATFORM_UNLOCK_SCORE = 600;
	constexpr int HIGH_VARIETY_SCORE = 10000; //past this score, platform lean more on broken and moving
	constexpr float GAME_OVER_MARGIN = 700.f;   //how far the camera follows the fall before freezin

	//obstacles: 
	constexpr int OBSTACLE_UNLOCK_SCORE = 1200; //birds start appear after this score
	constexpr float BIRD_WIDTH = 130.f;
	constexpr float BIRD_HEIGHT = 130.f;
	constexpr int ROCK_UNLOCK_SCORE = 3000;
	constexpr int ROCK_WAVE_INTERVAL = 1500;   //after 3000 every 1500 the rock appear
	constexpr float ROCK_WARNING_DURATION = 1.2f;   //seconds of warning before rock falls
	constexpr int BEE_NEST_UNLOCK_SCORE = 2000; 
	constexpr float BEE_NEST_TRIGGER_DISTANCE = 400.f; //nest countdown

	constexpr float MIN_OBSTACLE_PAGE_GAP = 3.5f;   //minimum page of vertical climb required between obstacle
	
	float randomSpacing(int currentScore) {
		float minSpacing = (currentScore >= DIFFICULTY_SCORE) ? HARD_MIN_SPACING : EASY_MIN_SPACING;
		float maxSpacing = (currentScore >= DIFFICULTY_SCORE) ? HARD_MAX_SPACING : EASY_MAX_SPACING;
		float range = maxSpacing - minSpacing;
		return minSpacing + static_cast<float>(std::rand() % static_cast<int>(range + 1));
	}
}

PlayState::PlayState(Game& game) : game(game),
	player(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 100.f), 
	bulletPool(20),     //pool of 20 reusable bullets
	cameraCenterY(WINDOW_HEIGHT / 2.f),
	bestHeightY(WINDOW_HEIGHT - 100.f), 
	score(0), 
	
	highScoreLineY(WINDOW_HEIGHT - 100.f - game.getHighScore() * 2.f),
	highScoreSavedThisRun(false),

	isGameOver(false), 
	isFallingFromBirdHit(false),
	birdsSpawnedThisBlock(0),
	currentScoreBlock(0),
	obstacleSpawnTimer(3.f),

	cameraFollowingDown(false),
	fallStartBottomY(0.f),
	fontLoaded(false) {
	
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	springUnlockScore = 300 + (std::rand() % 201);     //random threshold each game
	maxSpringThisGame = 2 + (std::rand() % 2);
	springSpawnedSoFar = 0; 
	obstacleSpawnTimer = 3.f;   //seconds until first bird is even considered
	std::cerr << "Spring unlock score: " << springUnlockScore << ", max springs: " << maxSpringThisGame << std::endl;

 	//font 	
	fontLoaded = font.loadFromFile("resources/LuckiestGuy.ttf");
	if (!fontLoaded) {	
		std::cerr << "Warning: could not load font -- but gameplay continues normally." << std::endl;
	}

	//background
	sf::Texture& backgroundTexture = ResourceManager::getInstance().getTexture("resources/playbackground.png");
	backgroundSprite.setTexture(backgroundTexture);
	sf::Vector2u texSize = backgroundTexture.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		backgroundSprite.setScale(WINDOW_WIDTH / texSize.x, WINDOW_HEIGHT / texSize.y); 
	}

		//ground floor: with the bottom 
	sf::Texture& groundTexture = ResourceManager::getInstance().getTexture("resources/ground_background.png");
	groundSprite.setTexture(groundTexture);
	sf::Vector2u groundSize = groundTexture.getSize();
	if (groundSize.x > 0 && groundSize.y > 0) {
		groundSprite.setScale(WINDOW_WIDTH / groundSize.x, WINDOW_HEIGHT / groundSize.y);
	}
	groundSprite.setPosition(0.f, 0.f);
	
	scoreText.setFont(font);
	scoreText.setCharacterSize(24);
	scoreText.setFillColor(sf::Color::Black);
	scoreText.setPosition(10.f, 10.f);

	//world height matching players saved best
	highScoreLineY = (WINDOW_HEIGHT - 100.f) - 2.f * static_cast<float>(game.getHighScore());
	
	//banana icon + running total
	sf::Texture& bananaIconTexture = ResourceManager::getInstance().getTexture("resources/banana.png");
	bananaIconSprite.setTexture(bananaIconTexture);
	bananaIconSprite.setScale(0.5f, 0.5f);
	bananaIconSprite.setPosition(10.f, 40.f);

	bananaCountText.setFont(font);
	bananaCountText.setCharacterSize(20);
	bananaCountText.setFillColor(sf::Color::Black);
	bananaCountText.setPosition(45.f, 42.f);
		
	//live ammo count 
	sf::Texture& ammoIconTexture = ResourceManager::getInstance().getTexture("resources/banana_coin.png");
	ammoIconSprite.setTexture(ammoIconTexture);
	sf::Vector2u ammoIconTexSize = ammoIconTexture.getSize();
	if (ammoIconTexSize.x > 0 && ammoIconTexSize.y > 0) {
		float targetWidth = 44.f;  //match banana icon
		float scale = targetWidth / ammoIconTexSize.x;
		ammoIconSprite.setScale(scale, scale);
	}
	ammoIconSprite.setPosition(10.f, 65.f);

	ammoText.setFont(font);
	ammoText.setCharacterSize(20);
	ammoText.setFillColor(sf::Color::Black);
	ammoText.setPosition(45.f, 67.f);
	
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(48);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setString("Game Over");
	sf::FloatRect goBounds = gameOverText.getLocalBounds();
	gameOverText.setOrigin(goBounds.left + goBounds.width / 2.f, goBounds.top + goBounds.height / 2.f);
	gameOverText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 120.f);

	sf::Vector2f gameOverButtonSize(260.f, 75.f);
	float gameOverButtonX = (WINDOW_WIDTH - gameOverButtonSize.x) / 2.f;
	gameOverButtons.reserve(2);

	gameOverButtons.emplace_back(font, "Play", sf::Vector2f(gameOverButtonX, WINDOW_HEIGHT / 2.f - 40.f),
		gameOverButtonSize, "resources/button_play.png"); 
	gameOverButtons.back().setOnClick([this]() {
		restartGame();
	});

	gameOverButtons.emplace_back(font, "Exit", sf::Vector2f(gameOverButtonX, WINDOW_HEIGHT / 2.f + 45.f), 
		gameOverButtonSize, "resources/button_exit.png");
	gameOverButtons.back().setOnClick([this]() {
		this->game.requestQuit();
	});
	
//pause icon 
	pauseIconBounds = sf::FloatRect(WINDOW_WIDTH - 40.f, 10.f, 30.f, 28.f);
	
	pausedText.setFont(font);
	pausedText.setCharacterSize(48);
	pausedText.setFillColor(sf::Color(30, 30, 30));
	pausedText.setString("PAUSED");
	sf::FloatRect pausedBounds = pausedText.getLocalBounds();
	pausedText.setOrigin(pausedBounds.left + pausedBounds.width / 2.f, pausedBounds.top + pausedBounds.height / 2.f);
	pausedText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 80.f);

	sf::Vector2f resumeButtonSize(220.f, 65.f);
	
	sf::Texture& resumeTexture = ResourceManager::getInstance().getTexture("resources/banana_bread.png");
	resumeSprite.setTexture(resumeTexture);
	sf::Vector2u resumeTexSize = resumeTexture.getSize();
	if (resumeTexSize.x > 0 && resumeTexSize.y > 0) {
		float resumeWidth = 220.f;
		float resumeScale = resumeWidth / resumeTexSize.x;
		resumeSprite.setScale(resumeScale, resumeScale);
		resumeSprite.setOrigin(resumeTexSize.x / 2.f, resumeTexSize.y / 2.f);
	}
	resumeSprite.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f + 35.f);

	resumeText.setFont(font);
	resumeText.setCharacterSize(22);
	resumeText.setFillColor(sf::Color(60, 45, 25));
	resumeText.setString("Resume");
	sf::FloatRect resumeBounds = resumeText.getLocalBounds();
	resumeText.setOrigin(resumeBounds.left + resumeBounds.width / 2.f, resumeBounds.top + resumeBounds.height / 2.f);
	resumeText.setPosition(resumeSprite.getPosition());
	
	// best score shown
	finalScoreText.setFont(font);
	finalScoreText.setCharacterSize(18);
	finalScoreText.setFillColor(sf::Color::Black);
	finalScoreText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 75.f);

	finalHighScoreText.setFont(font);
	finalHighScoreText.setCharacterSize(18);
	finalHighScoreText.setFillColor(sf::Color(80, 60, 20));
	finalHighScoreText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 50.f);


	gameView.setSize(WINDOW_WIDTH, WINDOW_HEIGHT);	
	gameView.setCenter(WINDOW_WIDTH / 2.f, cameraCenterY);
	
	spawnPlatforms();
} 

void PlayState::onEnter() {
	std::cerr << "Entered Play State" << std::endl;
}

void PlayState::spawnPlatforms() {
	platforms.clear();
	
	//start under player and space platform evenly going upward
	float y = WINDOW_HEIGHT - 40.f;
	
	for (int i = 0; i < PLATFORM_COUNT; ++i) {
		Platform platform;
		platform.shape.setSize({PLATFORM_WIDTH, PLATFORM_HEIGHT});
		platform.shape.setFillColor(sf::Color(139, 69, 19)); //brown

		float x = (i == 0) ? (WINDOW_WIDTH / 2.f - PLATFORM_WIDTH / 2.f) 
			: static_cast<float>(std::rand() % static_cast<int>(WINDOW_WIDTH - PLATFORM_WIDTH));

		platform.shape.setPosition(x, y);

		if (i != 0 && score >= springUnlockScore && std::rand() % 10 == 0) {
			platform.attachedPowerUp = std::make_unique<Spring>( 
				sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 10.f, y - 20.f));
		} else if (i != 0 && score >= BANANA_UNLOCK_SCORE && std::rand() % 12 == 0) {
			platform.attachedPowerUp = std::make_unique<Banana>(
				sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 11.f, y - 22.f));
		} else if (i != 0 && score >= SHIELD_UNLOCK_SCORE && std::rand() % 20 == 0) {
			platform.attachedPowerUp = std::make_unique<Shield>(
				sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 13.f, y - 26.f));
		} 

		platforms.push_back(std::move(platform));
		y -= randomSpacing(score);
	} 
}

void PlayState::recyclePlatforms() {
	// find current highest platform so new ones can be placed above it
	float highestPlatformY = platforms.front().shape.getPosition().y;
	for (const auto& platform : platforms) {
		highestPlatformY = std::min(highestPlatformY, platform.shape.getPosition().y);
	}

	float visibleBottom = cameraCenterY + WINDOW_HEIGHT / 2.f;
	
	for (auto& platform : platforms) {
		if (platform.shape.getPosition().y > visibleBottom + PLATFORM_HEIGHT) {
			highestPlatformY -= randomSpacing(score);
			float x = static_cast<float>(std::rand() % static_cast<int>(WINDOW_WIDTH - PLATFORM_WIDTH));			
			platform.shape.setPosition(x, highestPlatformY);

			//decide behavior first
			if (score >= HIGH_VARIETY_SCORE) {
				int roll = std::rand() % 10;
				if (score >= BREAKING_PLATFORM_UNLOCK_SCORE && roll < 4) {
					platform.behavior = std::make_unique<BreakingBehavior>();
				} else if (score >= MOVING_PLATFORM_UNLOCK_SCORE && roll < 8) {
					float speed = 80.f + static_cast<float>(std::rand() % 60);
					float minX = 0.f;
					float maxX = static_cast<float>(WINDOW_WIDTH);
					platform.behavior  = std::make_unique<MovingBehavior>(minX, maxX, speed);
				} else {
					platform.behavior = std::make_unique<NormalBehavior>();
				}
			} else if (score >= BREAKING_PLATFORM_UNLOCK_SCORE && std::rand() % 10 == 0) {
				platform.behavior = std::make_unique<BreakingBehavior>();
			} else if (score >= MOVING_PLATFORM_UNLOCK_SCORE && std::rand() % 8 == 0) {
				float speed = 80.f + static_cast<float>(std::rand() % 60);
				float minX = 0.f;
				float maxX = static_cast<float>(WINDOW_WIDTH);
				platform.behavior = std::make_unique<MovingBehavior>(minX, maxX, speed);
			} else {
				platform.behavior = std::make_unique<NormalBehavior>();
			}

			// re-roll the attached power-up fresha t this paltform new position 
			// decide power-up, gated on isMoving()
			if (platform.behavior->isMoving()) {
				platform.attachedPowerUp = nullptr;
			} else if  (score >= springUnlockScore && springSpawnedSoFar < maxSpringThisGame && 
				std::rand() % 10 == 0) {
				platform.attachedPowerUp = std::make_unique<Spring>(
					sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 15.f, highestPlatformY - 15.f));
			} else if (score >= BANANA_UNLOCK_SCORE && std::rand() % 12 == 0) {
				platform.attachedPowerUp = std::make_unique<Banana>(
					sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 11.f, highestPlatformY - 22.f));
			} else if (score >= SHIELD_UNLOCK_SCORE && std::rand() % 20 == 0) {
				platform.attachedPowerUp = std::make_unique<Shield>( 
					sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 13.f, highestPlatformY - 26.f));
			} else { 
				platform.attachedPowerUp = nullptr;
			}
			
		}
	} 
}

void PlayState::handleCollisions(float previousPlayerBottom) {
	if (isFallingFromBirdHit) {
		return;      // ignore all platform - player falls straight through
	}

	if (player.getVelocityY() <= 0.f) {
		return;
	}
	
	sf::FloatRect playerBounds = player.getBounds();

	for (auto& platform : platforms) {
		if (platform.checklanding(playerBounds, previousPlayerBottom)) {
			sf::Vector2f pos = player.getPosition();
			float newCenterY = platform.getTop() - playerBounds.height / 2.f;
			player.setPosition(pos.x, newCenterY);
			player.jump();
			platform.notifyLanded(); 
			break;
		}
	}
}
void PlayState::checkPowerUps() {
	sf::FloatRect playerBounds = player.getBounds();
	
	for (auto& platform : platforms) {
		if (!platform.attachedPowerUp) {
			continue;
		} 
		if (playerBounds.intersects(platform.attachedPowerUp->getBounds())) {
			platform.attachedPowerUp->apply(player);      //polymorphic - each power up definesits own effect
			if (platform.attachedPowerUp->consumedOnUse()) {
				platform.attachedPowerUp.reset();
			} 
		}
	}
}
	

void PlayState::spawnObstacles(float deltaTime) {
	obstacleSpawnTimer -= deltaTime;

	int thisBlock = score / 20000;
	if (thisBlock != currentScoreBlock) {
		currentScoreBlock = thisBlock;
		birdsSpawnedThisBlock = 0;
	}

	if (birdsSpawnedThisBlock >= 12) {
		return;     //hit the cap for this 20,000 score block
	}

	if (score < OBSTACLE_UNLOCK_SCORE ||  !obstacles.empty() || obstacleSpawnTimer > 0.f) {
		return;
	}

	float topOfView = cameraCenterY - WINDOW_HEIGHT / 2.f;
	//consider space cleary above the player current position
	float upperBound = topOfView - 100.f;
	float lowerBound = player.getPosition().y - 150.f;
		
	if (lowerBound <= upperBound) {
		obstacleSpawnTimer = 0.3f;   // not enough room above - check again
		return;
	}

	const int MAX_ATTEMPTS = 30;
	const float MARGIN = 50.f; //requred clearnace around the bird
	const float CLEARED_SPOT_RADIUS = 220.f;
	
	for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
		//pick x anywhere acroos width, guarantee open space
		float x = 30.f + static_cast<float>(std::rand() % static_cast<int>(WINDOW_WIDTH - BIRD_WIDTH - 60.f));
		float y = upperBound + static_cast<float>(std::rand() % static_cast<int>(lowerBound - upperBound));
	
		//check actual rectangle overlap against every platform (not top or squeeze)
		sf::FloatRect candidateBounds(x - MARGIN, y - MARGIN, BIRD_WIDTH + MARGIN * 2.f, BIRD_HEIGHT + MARGIN *2.f);
		bool overlapsAnyPlatform = false;
		for (const auto& platform : platforms) {
			sf::FloatRect platBounds = platform.shape.getGlobalBounds();
		
			if (dynamic_cast<MovingBehavior*>(platform.behavior.get())) {
				platBounds.left = 0.f;
				platBounds.width = WINDOW_WIDTH;
			}

			if (candidateBounds.intersects(platBounds)) {
				overlapsAnyPlatform = true;
				break;
			}
		} 

		//new changes, check if nest overlap with the bird
		bool overlapsAnyObstacle = false;
		for (const auto& existingObstacle : obstacles) {
			if (candidateBounds.intersects(existingObstacle->getBounds())) {
				overlapsAnyObstacle = true;
				break;
			}
		} 
	
		bool tooCloseToRecentlyCleared = false;
		for (const auto& spot : recentlyClearedSpots) {
			if (std::abs(spot.first - y) < CLEARED_SPOT_RADIUS) {
				tooCloseToRecentlyCleared = true;
				break;
			}
		}
		
		bool tooCloseToLastObstacle = std::abs(lastObstacleSpawnY - y) < MIN_OBSTACLE_PAGE_GAP * WINDOW_HEIGHT;

		if (!overlapsAnyPlatform && !overlapsAnyObstacle && !tooCloseToRecentlyCleared && 
			!tooCloseToLastObstacle) {
			obstacles.push_back(ObstacleFactory::createBird(sf::Vector2f(x, y)));
			birdsSpawnedThisBlock++;
			obstacleSpawnTimer = 10.f + static_cast<float>(std::rand() % 8);
			return;
		}
	}
	obstacleSpawnTimer = 0.3f;  //no valid spot - try again
}	

void PlayState::spawnBeeNest(float deltaTime) {
	beeNestSpawnTimer -= deltaTime;

	if (score < BEE_NEST_UNLOCK_SCORE || beeNestSpawnTimer > 0.f) {
		return;
	}

	float topOfView = cameraCenterY - WINDOW_HEIGHT / 2.f;
	float upperBound = topOfView - 100.f;
	float lowerBound = player.getPosition().y - 150.f;
	
	if (lowerBound <= upperBound) {
		beeNestSpawnTimer = 0.3f;
		return;
	} 

	const int MAX_ATTEMPTS = 30;
	const float MARGIN = 50.f;
	const float CLEARED_SPOT_RADIUS = 220.f;
	const float NEST_WIDTH = 45.f;
	const float NEST_HEIGHT = 45.f;
	
	for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
		float x;
		bool spawnOnLeft = (std::rand() % 2 == 0);	
		float edgeMargin = 20.f;
		float edgeVariance = 15.f;   // small random wiggle

		if (spawnOnLeft) {
			x = edgeMargin + static_cast<float>(std::rand() % static_cast<int>(edgeVariance));
		} else {
			x = WINDOW_WIDTH - NEST_WIDTH - edgeMargin - static_cast<float>(std::rand() % static_cast<int>(edgeVariance));
		} 
		
		float y = upperBound + static_cast<float>(std::rand() % static_cast<int>(lowerBound - upperBound));

		sf::FloatRect candidateBounds(x - MARGIN, y - MARGIN, NEST_WIDTH + MARGIN * 2.f, NEST_HEIGHT + MARGIN * 2.f);
		bool overlapsAnyPlatform = false;
		for (const auto& platform : platforms) {	
			sf::FloatRect platBounds = platform.shape.getGlobalBounds();
		
			if (dynamic_cast<MovingBehavior*>(platform.behavior.get())) {
				platBounds.left = 0.f;
				platBounds.width = WINDOW_WIDTH;
			}

			if (candidateBounds.intersects(platBounds)) {
				overlapsAnyPlatform = true;
				break;
			}
		}
		bool overlapsAnyObstacle = false;
		for (const auto& existingObstacle : obstacles) {
			if (candidateBounds.intersects(existingObstacle->getBounds())) {
				overlapsAnyObstacle = true;
				break;
			}
		}
	
		bool tooCloseToRecentlyCleared = false;
		for (const auto& spot : recentlyClearedSpots) {
			if (std::abs(spot.first - y) < CLEARED_SPOT_RADIUS) {
				tooCloseToRecentlyCleared = true;
				break;
			}
		}

		bool tooCloseToLastObstacle = std::abs(lastObstacleSpawnY - y) < MIN_OBSTACLE_PAGE_GAP * WINDOW_HEIGHT;

		if (!overlapsAnyPlatform && !overlapsAnyObstacle && !tooCloseToRecentlyCleared
			&& !tooCloseToLastObstacle) {
			obstacles.push_back(ObstacleFactory::createBeeNest(sf::Vector2f(x,y)));
			beeNestSpawnTimer = 15.f + static_cast<float>(std::rand() % 10);
			return;
		}
	}
	beeNestSpawnTimer = 0.3f;
}

void PlayState::spawnRockWave() {
	float topOfView = cameraCenterY - WINDOW_HEIGHT / 2.f;
	lastObstacleSpawnY = topOfView; //treat whole wave as one spawn event for spacing purpose
	int rockCount = 3 + (std::rand() % 2); //randomly 3 of 4 
	const int MAX_ATTEMPTS = 15;
	const float ROCK_CLEARANCE = 90.f;   //minimum distance from existing obstacle

	for (int i = 0; i < rockCount; ++i) {
		for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
			float x = static_cast<float>(std::rand() % static_cast<int>(WINDOW_WIDTH - 40.f));
			float y = topOfView - 100.f - static_cast<float>(std::rand() % 500);
		
			bool tooClose = false;
			for (const auto& existingObstacle : obstacles) {
				sf::FloatRect existingBounds = existingObstacle->getBounds();
				float dx = (existingBounds.left + existingBounds.width / 2.f) - x;
				float dy = (existingBounds.top + existingBounds.height / 2.f) - y;
				if (std::sqrt(dx * dx * dy * dy) < ROCK_CLEARANCE) {
					tooClose = true;
					break; 
				}
			}

			if (!tooClose) {
				float speed = 115.f + static_cast<float>(std::rand() % 135);
				obstacles.push_back(ObstacleFactory::createFallingRock(sf::Vector2f(x, y), speed));
				break;
			}
			// if every attempt failed, this rock is simply skipped rather than forced in bad spot
		}
	}
}

void PlayState::recycleObstacles() {
	float visibleBottom = cameraCenterY + WINDOW_HEIGHT / 2.f;
	// remove obstacles that have scorred below visible area
	obstacles.erase ( 
		std::remove_if(obstacles.begin(), obstacles.end(), [visibleBottom](const std::unique_ptr<Obstacle>& o) {	
		return o->getBounds().top > visibleBottom + 40.f;
	}),
	obstacles.end());
}

void PlayState::checkObstacles(float previousPlayerBottom) {
	sf::FloatRect playerBounds = player.getBounds();
	float currentBottom = playerBounds.top + playerBounds.height;

	for (auto& obstacle : obstacles) {
		sf::FloatRect obstacleBounds = obstacle->getBounds();

		if (!playerBounds.intersects(obstacleBounds)) {
			continue;
		} 

		bool landedOnTop = 
			player.getVelocityY() > 0.f && previousPlayerBottom <= obstacleBounds.top && 
			currentBottom >= obstacleBounds.top;
		if (landedOnTop) {
			if (obstacle->onLandedOnTop(player)) {
				isFallingFromBirdHit = true;
				player.setVelocityY(500.f);
			}
		} else {
			if (obstacle->onHitFromBelow(player)) {
				isFallingFromBirdHit = true;
				player.setVelocityY(500.f);   //force a hard downward fall
			} 
		} 
	}
}

void PlayState::checkBulletHits() {
	//check each active bullet from pool against every obstacle on hit
	for (auto& bullet : bulletPool.getBullets()) {
		if (!bullet.isActive()) {
			continue;
		} 
	
		for (auto it = obstacles.begin(); it != obstacles.end(); ++it) {
			if (!(*it)->isShootable()) {
				if ((*it)->bouncesBullets() && bullet.getBounds().intersects((*it)->getBounds())) {
					bullet.bounce();
					break;    //bullet bounce, obstacle intact
				}
				continue;
			}
			
			if (bullet.getBounds().intersects((*it)->getBounds())) {
				sf::FloatRect deadBounds = (*it)->getBounds();
				recentlyClearedSpots.push_back({deadBounds.top + deadBounds.height / 2.f, 3.f});
				bullet.deactivate();
				obstacles.erase(it);
				break;    // bullet is used up and move to next one
			}
		}
	}
}



void PlayState::updateCamera() {
	float frozenBottom = cameraCenterY + WINDOW_HEIGHT / 2.f;
	
	//player falls past the bottom view, start smoothly following down
	if (!cameraFollowingDown && player.getPosition().y > frozenBottom) {
		cameraFollowingDown = true;
		fallStartBottomY = frozenBottom;
	}
	if (cameraFollowingDown) {
		//track the player while falling
		cameraCenterY = player.getPosition().y - WINDOW_HEIGHT / 2.f + 100.f;
	} 
	else { 
		// climb, camera moves up
		float desiredCenterY = player.getPosition().y;
		if (desiredCenterY < cameraCenterY) {
			cameraCenterY = desiredCenterY;
		}
	}

	gameView.setCenter(WINDOW_WIDTH / 2.f, cameraCenterY);
}

void PlayState::updateScore() {
	bestHeightY = std::min(bestHeightY, player.getPosition().y);
	float startY = WINDOW_HEIGHT - 100.f;
	score = std::max(0, static_cast<int>((startY - bestHeightY) / 2.f));

	scoreText.setString("Score: " + std::to_string(score));
	bananaCountText.setString(std::to_string(player.getBananaCollected()));
	ammoText.setString(std::to_string(player.getAmmo()));
}

void PlayState::restartGame() {
	player.reset(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 100.f);

	springUnlockScore = 300 + (std::rand() % 201);
	maxSpringThisGame = 2 + (std::rand() % 2);
	springSpawnedSoFar = 0;

	spawnPlatforms();
	obstacles.clear();
	isFallingFromBirdHit = false;
	birdsSpawnedThisBlock = 0;
	currentScoreBlock = 0;
	obstacleSpawnTimer = 3.f;

	cameraCenterY = WINDOW_HEIGHT / 2.f;
	gameView.setCenter(WINDOW_WIDTH / 2.f, cameraCenterY);

	bestHeightY = WINDOW_HEIGHT - 100.f;
	score = 0;
	
	highScoreLineY = WINDOW_HEIGHT - 100.f - game.getHighScore() * 2.f;
	highScoreSavedThisRun = false;

	isGameOver = false;
	cameraFollowingDown = false;
	fallStartBottomY = 0.f;
}

void PlayState::handleEvent(const sf::Event& event) {
	if (isPaused) {
		if (event.type == sf::Event::MouseMoved) {
			sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
			bool hovered = resumeSprite.getGlobalBounds().contains(mousePos);
			resumeSprite.setColor(hovered ? sf::Color(230, 230, 230) : sf::Color::White);
		}
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>
				(event.mouseButton.y));
			if (resumeSprite.getGlobalBounds().contains(mousePos)) {
				isPaused = false;
			}
		}
		return;

	}
	if (!isGameOver && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>
			(event.mouseButton.y));
		if (pauseIconBounds.contains(mousePos))	{
			isPaused = true;
			return;
		}
	}
	if (isGameOver) {
		if (event.type == sf::Event::MouseMoved) {
			sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
			for (auto& button : gameOverButtons) {
				button.setHovered(button.contains(mousePos));
			}
		}
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>
				(event.mouseButton.y));
			for (auto& button : gameOverButtons) {
				button.handleClick(mousePos);
			}
		}
	}
}

void PlayState::update(float deltaTime) {
	if (isPaused) {
		return;
	}

	if (isGameOver) {        //game over show -- freeze camera, let plater fall under gavity
		return;      // freeze gameplay until player restarts
	}

	sf::FloatRect boundsBeforeMove = player.getBounds();
	float previousBottom = boundsBeforeMove.top + boundsBeforeMove.height;

	player.handleInput();
	player.update(deltaTime, WINDOW_WIDTH);

	for (auto& platform : platforms) {
		platform.update(deltaTime);
	}	

	handleCollisions(previousBottom);
	if (!isFallingFromBirdHit) {
		checkPowerUps();
	}
	recyclePlatforms();

	//fire bullet on fresh space press, if player has ammo
	if (player.consumeFirePressed() && player.hasAmmo()) {
		player.useAmmo();
		bulletPool.fire(sf::Vector2f(player.getPosition().x - 5.f, player.getPosition().y - 20.f));
	}
	bulletPool.update(deltaTime, cameraCenterY - WINDOW_HEIGHT / 2.f);
	checkBulletHits();

	for (auto& obstacle : obstacles) {
		if (auto* nest = dynamic_cast<BeeNest*>(obstacle.get())) {
			sf::FloatRect bounds = nest->getBounds();		
			float nestCenterY = bounds.top + bounds.height / 2.f;
			float verticalDistance = std::abs(nestCenterY - player.getPosition().y);
			nest->setPlayerNear(verticalDistance <= BEE_NEST_TRIGGER_DISTANCE);
		}
		obstacle->update(deltaTime); //delegates each obstacle own movement strategy
	}

	//release any nest thats ready to spawn its swarm
	std::vector<std::pair<sf::Vector2f, BeeNest*>> nestsToRelease;
	for (auto& obstacle : obstacles) {
		if (auto* nest = dynamic_cast<BeeNest*>(obstacle.get())) {
			if (nest->isReadyToSpawn()) {
				sf::FloatRect bounds = nest->getBounds();
				nestsToRelease.push_back({sf::Vector2f(bounds.left, bounds.top), nest});
			}
		} 
	}

	for (auto& pair : nestsToRelease) {
		sf::Vector2f nestPos = pair.first;
		BeeNest* nest = pair.second;
		int count = nest->getBeeCount();

		bool nestOnLeft = nestPos.x < WINDOW_WIDTH / 2.f;

		for (int i = 0; i < count; ++i) {
			float angleDegrees;
			if (nestOnLeft) {
				angleDegrees = -60.f + static_cast<float>(std::rand() % 120);
			} else {
				angleDegrees = 120 + static_cast<float>(std::rand() % 120);
			}
			float angle = angleDegrees * 3.14159f / 180.f;
			float speed = 45.f + static_cast<float>(std::rand() % 55);  //bees pace

			float offsetX = static_cast<float>(std::rand() % 30) - 15.f;
			float offsetY = static_cast<float>(std::rand() % 30) - 15.f;
			sf::Vector2f beePos(nestPos.x + offsetX, nestPos.y + offsetY);

			obstacles.push_back(ObstacleFactory::createBee(nestPos, angle, speed));
		}
		nest->markSpawned();
	}

	for (auto& spot : recentlyClearedSpots) {
		spot.second -= deltaTime;
	}

	recentlyClearedSpots.erase(
		std::remove_if(recentlyClearedSpots.begin(), recentlyClearedSpots.end(), 
			[](const auto & spot) {
				return spot.second <= 0.f; 
		}), 
		recentlyClearedSpots.end()
	);

	checkObstacles(deltaTime);
	spawnObstacles(deltaTime);
	spawnBeeNest(deltaTime);
	
	bool rockTooCloseToLastObstacle = std::abs(lastObstacleSpawnY - cameraCenterY) < MIN_OBSTACLE_PAGE_GAP 
		* WINDOW_HEIGHT;

	if (score >= nextRockWaveScore && !rockWarningActive && !rockTooCloseToLastObstacle) {
		rockWarningActive = true;
		rockWarningTimer = ROCK_WARNING_DURATION;
		nextRockWaveScore += ROCK_WAVE_INTERVAL;
	}
	if (rockWarningActive) {
		rockWarningTimer -= deltaTime;
		if (rockWarningTimer <= 0.f) {
			rockWarningActive = false;
			spawnRockWave();
		}
	}
		
	recycleObstacles();


	updateCamera();
	updateScore();

	//game over: player fallen below what camera can see
	if (cameraFollowingDown && player.getPosition().y > fallStartBottomY + GAME_OVER_MARGIN) {
		isGameOver = true;
		player.showHurt();
		float bottomOfScreenY = cameraCenterY + WINDOW_HEIGHT / 2.f - 30.f;
		player.setPosition(player.getPosition().x, bottomOfScreenY);

		if (!highScoreSavedThisRun) {
			game.updateHighScore(score);
			highScoreSavedThisRun = true;
		}
	}
}

void PlayState::render(sf::RenderWindow& window) {
	window.setView(gameView);
	window.clear();

	window.draw(groundSprite);

	float topOfView = cameraCenterY - WINDOW_HEIGHT / 2.f;
	float tileIndex = std::floor(topOfView / WINDOW_HEIGHT);
	float tileY = tileIndex * WINDOW_HEIGHT;	

	auto drawTile = [&](float index, float y) {
		if (index == 0.f) return;  //skip ground floor
		backgroundSprite.setPosition(0.f, y);
		window.draw(backgroundSprite);
	};
		
	drawTile(tileIndex, tileY);
	drawTile(tileIndex + 1.f, tileY + WINDOW_HEIGHT);

	for (const auto& platform : platforms) {
		platform.draw(window);
	}

	for (const auto& obstacle : obstacles) {
		obstacle->draw(window);
	} 
	bulletPool.draw(window);
	player.draw(window);
	
	if (game.getHighScore() > 0) {	
		sf::VertexArray dashedLine(sf::Lines);
		float dashLength = 12.f;
		float gapLength = 8.f;
		for (float x = 0.f; x < WINDOW_WIDTH; x += dashLength + gapLength) {
			dashedLine.append(sf::Vertex(sf::Vector2f(x, highScoreLineY), sf::Color(200, 60, 60, 180)));
			dashedLine.append(sf::Vertex(sf::Vector2f(std::min(x +  dashLength, WINDOW_WIDTH), 
				highScoreLineY), sf::Color(200, 60, 60, 180)));
		}
		window.draw(dashedLine);
	}

	window.setView(window.getDefaultView());
	
	if (rockWarningActive) {
		float flashSpeed = 8.f;
		float pulse = (std::sin(rockWarningTimer * flashSpeed) + 1.f) / 2.f;
		sf::Uint8 alpha = static_cast<sf::Uint8>(60 + pulse * 100);

		sf::RectangleShape dimOverlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
		dimOverlay.setPosition(0.f, 0.f);
		dimOverlay.setFillColor(sf::Color(150, 0, 0, alpha));
		window.draw(dimOverlay); 
 	}

	if (!isGameOver) {
		window.draw(scoreText);
	// pause icon
		sf::RectangleShape pauseBar(sf::Vector2f(6.f, 22.f));
		pauseBar.setPosition(pauseIconBounds.left + 4.f, pauseIconBounds.top + 3.f);
		window.draw(pauseBar);
		pauseBar.setPosition(pauseIconBounds.left + 16.f, pauseIconBounds.top + 3.f);
		window.draw(pauseBar);

		if (isPaused) {
			sf::RectangleShape dim(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
			dim.setFillColor(sf::Color(0, 0, 0, 120));
			window.draw(dim);
			window.draw(pausedText);
			window.draw(resumeSprite);
			window.draw(resumeText);
		}

		window.draw(bananaIconSprite);
		window.draw(bananaCountText);
		window.draw(ammoIconSprite);
		window.draw(ammoText);
	}	
	
	if (isGameOver) {
		window.draw(gameOverText);

		finalScoreText.setString("Score: " + std::to_string(score));
		sf::FloatRect fsBounds = finalScoreText.getLocalBounds();
		finalScoreText.setOrigin(fsBounds.left + fsBounds.width / 2.f, fsBounds.top + fsBounds.height / 2.f);
		window.draw(finalScoreText);

		finalHighScoreText.setString("Best: " + std::to_string(game.getHighScore()));
		sf::FloatRect fhBounds = finalHighScoreText.getLocalBounds();
		finalHighScoreText.setOrigin(fhBounds.left + fhBounds.width / 2.f, fhBounds.top + fhBounds.height / 2.f);
		window.draw(finalHighScoreText);

	
		for (const auto& button : gameOverButtons) {
			button.draw(window);
		}
	}
}
