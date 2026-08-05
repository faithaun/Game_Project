#include "Player.hpp"
#include <iostream>
#include <algorithm>

//definition of tunable constants
const float Player::WIDTH = 40.f;
const float Player::HEIGHT = 40.f;
const float Player::GRAVITY = 900.f;         //pulls player down
const float Player::JUMP_VELOCITY = -520.f;      // negative = upward
const float Player::BIG_JUMP_VELOCITY = -780.f;   //stronger upward boose for spring
const float Player::MOVE_SPEED = 300.f;       // horizontal speed


//constructor: set up visual shape and initial state
Player::Player(float startX, float startY) : velocity(0.f, 0.f) {
	shape.setSize(sf::Vector2f(WIDTH, HEIGHT));
	shape.setOrigin(WIDTH / 2.f, HEIGHT / 2.f);
	shape.setPosition(startX, startY);
	
	if (!texture.loadFromFile("resources/monkey.png")) {
		std::cerr << "Warning: could not load player.png" << std::endl;
	}
	sprite.setTexture(texture);
	sf::Vector2u texSize = texture.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		float visualScale = 1.5f; // 50% bigger
		baseScaleX = visualScale * WIDTH / texSize.x;
		baseScaleY = visualScale * HEIGHT / texSize.y;
	}
	sprite.setScale(baseScaleX, baseScaleY);
	sprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
	sprite.setPosition(startX, startY);

	//shield bubble
	if (!bubbleTexture.loadFromFile("resources/bubble.png")) {
		std::cerr << "Warning: could not load bubble" << std::endl;
	}
	bubbleSprite.setTexture(bubbleTexture);
	sf::Vector2u bubbleTexSize = bubbleTexture.getSize();
	if (bubbleTexSize.x > 0 && bubbleTexSize.y > 0) {
		float monkeyRenderedWidth = std::abs(baseScaleX) * texSize.x;
		float monkeyRenderedHeight  = baseScaleY * texSize.y;
		constexpr float BUBBLE_MARGIN = 1.5f;  //bigger than monkey
		float bubbleScaleX = (monkeyRenderedWidth * BUBBLE_MARGIN) / bubbleTexSize.x;
		float bubbleScaleY = (monkeyRenderedHeight * BUBBLE_MARGIN) / bubbleTexSize.y;
		bubbleSprite.setScale(bubbleScaleX, bubbleScaleY);
	}
	bubbleSprite.setOrigin(bubbleTexSize.x / 2.f, bubbleTexSize.y / 2.f);
	bubbleSprite.setPosition(startX, startY);
	bubbleSprite.setColor(sf::Color(255, 255, 255, 110));  //translucent

	if (!hurtTexture.loadFromFile("resources/hurt_monkey.png")) {
		std::cerr << "Warning cound not load hurt monkey" << std::endl;
	}
}

//handle input: reads keuboard state, sets horizontal velocty
void Player::handleInput() {
	velocity.x = 0.f;
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { 
		velocity.x = -MOVE_SPEED;
		facingRight = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		velocity.x = MOVE_SPEED;
		facingRight = true;
	}
	
	//edge-detect space - only counts as press the frame it goes from up to down
	bool spaceIsDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
	if (spaceIsDown && !spaceWasDown) {
		firePressedThisFrame = true;
	}
	spaceWasDown = spaceIsDown;
} 

//apply gravity: add downwards acceleration to vertical velocity
void Player::applyGravity (float deltaTime) {	
	velocity.y += GRAVITY * deltaTime;
}

//jump
void Player::jump() {
	velocity.y = JUMP_VELOCITY;
}

void Player::bigJump() {
	velocity.y = BIG_JUMP_VELOCITY;
}

void Player::showHurt() {
	velocity = sf::Vector2f(0.f, 0.f);
	sprite.setTexture(hurtTexture);
	sf::Vector2u hurtSize = hurtTexture.getSize();
	if (hurtSize.x > 0 && hurtSize.y > 0) {
		float visualScale = 2.5f; 
		float scale = visualScale * std::min(WIDTH / (float)hurtSize.x, HEIGHT / (float)hurtSize.y);
		sprite.setScale(facingRight ? scale : -scale, scale);
		sprite.setOrigin(hurtSize.x / 2.f, hurtSize.y / 2.f);
	}
}

//update: applies gravity, integrate velocity into position, and wraps player left/right screen edges
void Player::update(float deltaTime, float windowWidth) {
	applyGravity(deltaTime);
	shape.move(velocity.x * deltaTime, velocity.y * deltaTime);

	sf::Vector2f pos = shape.getPosition();
	if (pos.x + WIDTH / 2.f < 0.f) {
		pos.x = windowWidth + WIDTH / 2.f;
	}
	else if (pos.x - WIDTH / 2.f > windowWidth) {
		pos.x = -WIDTH / 2.f;
	} 
	shape.setPosition(pos);
	sprite.setPosition(pos);
	sprite.setScale(facingRight ? baseScaleX : -baseScaleX, baseScaleY);
	bubbleSprite.setPosition(pos);

	if (shieldTimeRemaining > 0.f) {
		shieldTimeRemaining -= deltaTime;
		if (shieldTimeRemaining < 0.f) {
			shieldTimeRemaining = 0.f;
		}
	}
}

//draw
void Player::draw(sf::RenderWindow& window) const {	
	window.draw(sprite);	
	if (isShieldActive()) {
		window.draw(bubbleSprite);
	}
	window.draw(sprite);
} 

//Getter
sf::FloatRect Player::getBounds() const {
	return shape.getGlobalBounds();
}

sf::Vector2f Player::getPosition() const {
	return shape.getPosition();
}

float Player::getVelocityY() const {
	return velocity.y;
}


//mutators
void Player::setPosition(float x, float y) {
	shape.setPosition(x, y);
	sprite.setPosition(x, y);
	bubbleSprite.setPosition(x, y);
}

void Player::setVelocityY(float vy) {
	velocity.y = vy;
}

void Player::reset(float x, float y) {
	shape.setPosition(x, y);
	sprite.setPosition(x, y);
	velocity = sf::Vector2f(0.f, 0.f);
	ammo = 0;
	bananasCollected = 0;
	shieldTimeRemaining = 0.f;
	restoreIdleSprite();
}

void Player::restoreIdleSprite() {
	sprite.setTexture(texture);
	sf::Vector2u texSize = texture.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		sprite.setScale(facingRight ? baseScaleX : -baseScaleX, baseScaleY);
		sprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
	}

} 


//ammo/shooting
void Player::addAmmo(int amount) {
	ammo += amount;
}

bool Player::hasAmmo() const {
	return ammo > 0;
}

void Player::useAmmo() {
	if (ammo > 0) {
		ammo--;
	}
}

int Player::getAmmo() const {
	return ammo;
}

bool Player::consumeFirePressed() {
	bool wasPressed = firePressedThisFrame;
	firePressedThisFrame = false;
	return wasPressed;
}


void Player::recordBananaCollected() {
	bananasCollected++;
}

int Player::getBananaCollected() const {
	return bananasCollected;
}

void Player::activateShield(float duration) {
	shieldTimeRemaining = duration;
}


bool Player::isShieldActive() const {
	return shieldTimeRemaining > 0.f;
}

