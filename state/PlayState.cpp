#include "PlayState.hpp"
#include "../Game.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

namespace {
	constexpr float WINDOW_WIDTH = 540.f;
	constexpr float WINDOW_HEIGHT = 700.f;
	constexpr float PLATFORM_WIDTH = 70.f;
	constexpr float PLATFORM_HEIGHT = 15.f;
	constexpr int PLATFORM_COUNT = 12;                // 12 min platforms, per spec
	constexpr float PLATFORM_SPACING = 70.f;
	constexpr float GAME_OVER_MARGIN = 60.f; //how far below the camera before game over
}

PlayState::PlayState(Game& game) : game(game),
	player(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 100.f),
	cameraCenterY(WINDOW_HEIGHT / 2.f),
	bestHeightY(WINDOW_HEIGHT - 100.f), 
	score(0), 
	isGameOver(false), 
	fontLoaded(false) {
	
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	
	fontLoaded = font.loadFromFile("resources/LuckiestGuy.tff");
	if (!fontLoaded) {	
		std::cerr << "Warning: could not load font -- but gameplay continues normally." << std::endl;
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
		platforms.push_back(platform);

		y -= PLATFORM_SPACING;
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
			highestPlatformY -= PLATFORM_SPACING;
			float x = static_cast<float>(std::rand() % static_cast<int>(WINDOW_WIDTH - PLATFORM_WIDTH));
			platform.shape.setPosition(x, highestPlatformY);
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


void PlayState::updateCamera() {
	//camera targets players Y directly, but moves up
	float desiredCenterY = player.getPosition().y;
	if (desiredCenterY < cameraCenterY) {
		cameraCenterY = desiredCenterY;
	}
	gameView.setCenter(WINDOW_WIDTH / 2.f, cameraCenterY);
}

void PlayState::updateScore() {
	bestHeightY = std::min(bestHeightY, player.getPosition().y);
	float startY = WINDOW_HEIGHT - 100.f;
	score = std::max(0, static_cast<int>((startY - bestHeightY) / 10.f));
	if (score < 0) score = 0;

	scoreText.setString("Score: " + std::to_string(score));
}

void PlayState::restartGame() {
	player.reset(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 100.f);
	spawnPlatforms();

	cameraCenterY = WINDOW_HEIGHT / 2.f;
	gameView.setCenter(WINDOW_WIDTH / 2.f, cameraCenterY);

	bestHeightY = WINDOW_HEIGHT - 100.f;
	score = 0;
	isGameOver = false;
}

void PlayState::handleEvent(const sf::Event& event) {
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
		restartGame();
	} 
}

void PlayState::update(float deltaTime) {
	if (isGameOver) {
		return;      // freeze gameplay until player restarts
	}

	sf::FloatRect boundsBeforeMove = player.getBounds();
	float previousBottom = boundsBeforeMove.top + boundsBeforeMove.height;

	player.handleInput();
	player.update(deltaTime, WINDOW_WIDTH);
	
	handleCollisions(previousBottom);
	recyclePlatforms();
	updateCamera();
	updateScore();

	//game over: player fallen below what camera can see
	if (player.getPosition().y > cameraCenterY + WINDOW_HEIGHT / 2.f + GAME_OVER_MARGIN) {
		isGameOver = true;
	}
}

void PlayState::render(sf::RenderWindow& window) {
	window.setView(gameView);
	window.clear(sf::Color(135, 206, 235));   //sky blue

	for (const auto& platform : platforms) {
		window.draw(platform.shape);
	}

	player.draw(window);
	
	window.setView(window.getDefaultView());
	window.draw(scoreText);

	if (isGameOver) {
		window.draw(gameOverText);
		window.draw(restartText);
	}
}
