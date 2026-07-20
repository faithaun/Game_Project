#include "PlayState.hpp"
#include "../Game.hpp" 
#include "../powerups/Spring.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>

namespace {
	constexpr float WINDOW_WIDTH = 540.f;
	constexpr float WINDOW_HEIGHT = 700.f;

	constexpr float PLATFORM_WIDTH = 70.f;
	constexpr float PLATFORM_HEIGHT = 15.f;
	constexpr int PLATFORM_COUNT = 20;                // 12 min platforms, per spec

	constexpr float EASY_MIN_SPACING = 35.f;
	constexpr float EASY_MAX_SPACING = 55.f;
	constexpr float HARD_MIN_SPACING = 50.f;
	constexpr float HARD_MAX_SPACING = 80.f;
	constexpr int DIFFICULTY_SCORE = 150;

	constexpr float GAME_OVER_MARGIN = 700.f;


	float randomSpacing(int currentScore) {
		float minSpacing = (currentScore >= DIFFICULTY_SCORE) ? HARD_MIN_SPACING : EASY_MIN_SPACING;
		float maxSpacing = (currentScore >= DIFFICULTY_SCORE) ? HARD_MAX_SPACING : EASY_MAX_SPACING;
		float range = maxSpacing - minSpacing;
		return minSpacing + static_cast<float>(std::rand() % static_cast<int>(range + 1));
	}
}

PlayState::PlayState(Game& game) : game(game),
	player(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 100.f),
	cameraCenterY(WINDOW_HEIGHT / 2.f),
	bestHeightY(WINDOW_HEIGHT - 100.f), 
	score(0), 
	isGameOver(false), 
	cameraFollowingDown(false),
	fallStartBottomY(0.f),
	fontLoaded(false) {
	
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	springUnlockScore = 300 + (std::rand() % 201);     //random threshold each game
	maxSpringThisGame = 2 + (std::rand() % 2);
	springSpawnedSoFar = 0;
	std::cerr << "Spring unlock score: " << springUnlockScore << ", max springs: " << maxSpringThisGame << std::endl;

 	//font 	
	fontLoaded = font.loadFromFile("resources/LuckiestGuy.ttf");
	if (!fontLoaded) {	
		std::cerr << "Warning: could not load font -- but gameplay continues normally." << std::endl;
	}

	
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
			springSpawnedSoFar++;
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

			if (score >= springUnlockScore && springSpawnedSoFar < maxSpringThisGame && 
				std::rand() % 10 == 0) {
				platform.attachedPowerUp = std::make_unique<Spring>(
					sf::Vector2f(x + PLATFORM_WIDTH / 2.f - 10.f, highestPlatformY - 20.f));
				springSpawnedSoFar++;
			} else { 
				platform.attachedPowerUp = nullptr;
			}
		}
	} 
}

void PlayState::handleCollisions(float previousPlayerBottom) {
	if (player.getVelocityY() <= 0.f) {
		return;
	}
	
	sf::FloatRect playerBounds = player.getBounds();
	float currentBottom = playerBounds.top + playerBounds.height;

	for (const auto& platform : platforms) {
		sf::FloatRect platformBounds = platform.shape.getGlobalBounds();
		
		bool horizontalOverlap = 
			playerBounds.left + playerBounds.width > platformBounds.left &&
			playerBounds.left < platformBounds.left + platformBounds.width;
		
		//make landing reliable
		bool crossedTopThisFrame = 
			previousPlayerBottom <= platformBounds.top &&
			currentBottom >= platformBounds.top;

		if (horizontalOverlap && crossedTopThisFrame) {
			sf::Vector2f pos = player.getPosition();
			float newCenterY = platformBounds.top - playerBounds.height / 2.f;
			player.setPosition(pos.x, newCenterY);
			player.jump();
			break;         // only land on one platform per frame 
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
}

void PlayState::restartGame() {
	player.reset(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 100.f);

	springUnlockScore = 300 + (std::rand() % 201);
	maxSpringThisGame = 2 + (std::rand() % 2);
	springSpawnedSoFar = 0;

	spawnPlatforms();

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
	
	handleCollisions(previousBottom);
	checkPowerUps();
	recyclePlatforms();
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
		window.draw(platform.shape);
		if (platform.attachedPowerUp) {
			platform.attachedPowerUp->draw(window);
		}
	}

	player.draw(window);
	
	window.setView(window.getDefaultView());
	window.draw(scoreText);

	if (isGameOver) {
		window.draw(gameOverText);
		window.draw(restartText);
	}
}
