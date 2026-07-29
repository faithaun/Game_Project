#include "Player.hpp"

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
	shape.setFillColor(sf::Color::Green);
	shape.setOrigin(WIDTH / 2.f, HEIGHT / 2.f);
	shape.setPosition(startX, startY);
}

//handle input: reads keuboard state, sets horizontal velocty
void Player::handleInput() {
	velocity.x = 0.f;
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { 
		velocity.x = -MOVE_SPEED;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		velocity.x = MOVE_SPEED;
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
	
	if (shieldTimeRemaining > 0.f) {
		shieldTimeRemaining -= deltaTime;
		if (shieldTimeRemaining < 0.f) {
			shieldTimeRemaining = 0.f;
		}
	}
}

//draw
void Player::draw(sf::RenderWindow& window) const {	
	window.draw(shape);
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
}

void Player::setVelocityY(float vy) {
	velocity.y = vy;
}

void Player::reset(float x, float y) {
	shape.setPosition(x, y);
	velocity = sf::Vector2f(0.f, 0.f);
	ammo = 0;
	bananasCollected = 0;
	shieldTimeRemaining = 0.f;
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

