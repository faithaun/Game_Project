#include "PlayState.hpp"
#include "../Game.hpp" 
#include "../powerups/Spring.hpp"
#include "../powerups/Banana.hpp"
#include "../powerups/Shield.hpp"
#include "../obstacles/Bird.hpp"
#include "../platform/MovingBehavior.hpp"
#include "../platform/BreakingBehavior.hpp"
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
	constexpr float GAME_OVER_MARGIN = 700.f;   //how far the camera follows the fall before freezing
	
	//obstacles: 
	constexpr int OBSTACLE_UNLOCK_SCORE = 1200; //birds start appear after this score
	constexpr float BIRD_WIDTH = 40.f;
	constexpr float BIRD_HEIGHT = 30.f;


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
	if (!backgroundTexture.loadFromFile("resources/playbackground.png")) {
		std::cerr << "Warning: could not load resources/playbackground.png" << std::endl;
	}
	backgroundSprite.setTexture(backgroundTexture);
	sf::Vector2u texSize = backgroundTexture.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		backgroundSprite.setScale(WINDOW_WIDTH / texSize.x, WINDOW_HEIGHT / texSize.y); 
	} 

	scoreText.setFont(font);
	scoreText.setCharacterSize(24);
	scoreText.setFillColor(sf::Color::Black);
	scoreText.setPosition(10.f, 10.f);
	
	//banana icon + running total
	if (!bananaIconTexture.loadFromFile("resources/banana.png")) {
		std::cerr << "Warning: could not load resources/Banana.png" << std::endl;
	}
	bananaIconSprite.setTexture(bananaIconTexture);
	bananaIconSprite.setScale(0.5f, 0.5f);
	bananaIconSprite.setPosition(10.f, 40.f);

	bananaCountText.setFont(font);
	bananaCountText.setCharacterSize(20);
	bananaCountText.setFillColor(sf::Color::Black);
	bananaCountText.setPosition(45.f, 42.f);
		
	//live ammo count 
	ammoText.setFont(font);
	ammoText.setCharacterSize(22);
	ammoText.setFillColor(sf::Color::Black);
	ammoText.setPosition(WINDOW_WIDTH - 110.f, 10.f);
	
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(48);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setString("Game Over");
	sf::FloatRect goBounds = gameOverText.getLocalBounds();
	gameOverText.setOrigin(goBounds.left + goBounds.width / 2.f, goBounds.top + goBounds.height / 2.f);
	gameOverText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 30.f);

	restartText.setFont(font);
	restartText.setCharacterSize(20);
	restartText.setFillColor(sf::Color::Black);
	restartText.setString("Press R to Restart");
	sf::FloatRect rBounds = restartText.getLocalBounds();
	restartText.setOrigin(rBounds.left + rBounds.width / 2.f, rBounds.top + rBounds.height / 2.f);
	restartText.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f + 30.f);
	
	
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

			// re-roll the attached power-up fresha t this paltform new position
			if (score >= springUnlockScore && springSpawnedSoFar < maxSpringThisGame && 
				std::rand() % 10 == 0) {
				platform.attachedPowerUp = std::make_unique<Spring>(
					sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 10.f, highestPlatformY - 20.f));
			} else if (score >= BANANA_UNLOCK_SCORE && std::rand() % 12 == 0) {
				platform.attachedPowerUp = std::make_unique<Banana>(
					sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 11.f, highestPlatformY - 22.f));
			} else if (score >= SHIELD_UNLOCK_SCORE && std::rand() % 20 == 0) {
				platform.attachedPowerUp = std::make_unique<Shield>( 
					sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 13.f, highestPlatformY - 26.f));
			} else { 
				platform.attachedPowerUp = nullptr;
			}
			
			//re-roll platform behavior
			if (score >= BREAKING_PLATFORM_UNLOCK_SCORE && std::rand() % 10 == 0) {
				platform.behavior = std::make_unique<BreakingBehavior>();
			} else if (score >= MOVING_PLATFORM_UNLOCK_SCORE && std::rand() % 8 == 0) {
				float speed = 80.f + static_cast<float>(std::rand() % 60);
				float minX = 0.f; 
				float maxX = static_cast<float>(WINDOW_WIDTH);
				platform.behavior = std::make_unique<MovingBehavior>(minX, maxX, speed);
			} else {
				platform.behavior = std::make_unique<NormalBehavior>();
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
	const float MARGIN = 30.f; //requred clearnace around the bird

	for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
		//pick x anywhere acroos width, guarantee open space
		float x = 30.f + static_cast<float>(std::rand() % static_cast<int>(WINDOW_WIDTH - BIRD_WIDTH - 60.f));
		float y = upperBound + static_cast<float>(std::rand() % static_cast<int>(lowerBound - upperBound));
	
		//check actual rectangle overlap against every platform (not top or squeeze)
		sf::FloatRect candidateBounds(x - MARGIN, y - MARGIN, BIRD_WIDTH + MARGIN * 2.f, BIRD_HEIGHT + MARGIN *2.f);
		bool overlapsAnyPlatform = false;
		for (const auto& platform : platforms) {
			if (candidateBounds.intersects(platform.shape.getGlobalBounds())) {
				overlapsAnyPlatform = true;
				break;
			}
		}
		if (!overlapsAnyPlatform) {
			obstacles.push_back(std::make_unique<Bird>(sf::Vector2f(x, y)));
			birdsSpawnedThisBlock++;
			obstacleSpawnTimer = 10.f + static_cast<float>(std::rand() % 8);
			return;
		}
	}
	obstacleSpawnTimer = 0.3f;  //no valid spot - try again
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
			obstacle->onLandedOnTop(player);
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
			if (bullet.getBounds().intersects((*it)->getBounds())) {
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
	ammoText.setString("Ammo: " + std::to_string(player.getAmmo()));
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
	isGameOver = false;
	cameraFollowingDown = false;
	fallStartBottomY = 0.f;
}

void PlayState::handleEvent(const sf::Event& event) {
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
		restartGame();
	} 
}

void PlayState::update(float deltaTime) {
	if (isGameOver) {        //game over show -- freeze camera, let plater fall under gavity
		player.update(deltaTime, WINDOW_WIDTH);
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
	checkPowerUps();
	recyclePlatforms();

	//fire bullet on fresh space press, if player has ammo
	if (player.consumeFirePressed() && player.hasAmmo()) {
		player.useAmmo();
		bulletPool.fire(sf::Vector2f(player.getPosition().x - 5.f, player.getPosition().y - 20.f));
	}
	bulletPool.update(deltaTime, cameraCenterY - WINDOW_HEIGHT / 2.f);
	checkBulletHits();

	for (auto& obstacle : obstacles) {
		obstacle->update(deltaTime); //delegates each obstacle own movement strategy
	}
	checkObstacles(previousBottom);
	spawnObstacles(deltaTime);
	recycleObstacles();


	updateCamera();
	updateScore();

	//game over: player fallen below what camera can see
	if (cameraFollowingDown && player.getPosition().y > fallStartBottomY + GAME_OVER_MARGIN) {
		isGameOver = true;
	}
}

void PlayState::render(sf::RenderWindow& window) {
	window.setView(gameView);
	window.clear();

	float topOfView = cameraCenterY - WINDOW_HEIGHT / 2.f;
	float tileIndex = std::floor(topOfView / WINDOW_HEIGHT);
	float tileY = tileIndex * WINDOW_HEIGHT;	
	backgroundSprite.setPosition(0.f, tileY);
	window.draw(backgroundSprite);
	backgroundSprite.setPosition(0.f, tileY + WINDOW_HEIGHT);
	window.draw(backgroundSprite);

	for (const auto& platform : platforms) {
		platform.draw(window);
	}

	for (const auto& obstacle : obstacles) {
		obstacle->draw(window);
	} 
	bulletPool.draw(window);
	player.draw(window);
	
	window.setView(window.getDefaultView());
	window.draw(scoreText);
	window.draw(bananaIconSprite);
	window.draw(bananaCountText);
	window.draw(ammoText);

	if (isGameOver) {
		window.draw(gameOverText);
		window.draw(restartText);
	}
}
