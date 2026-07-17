#include "Player.hpp"

Player::Player(float startX, float startY) : shape(sf::Vector2f(WIDTH, HEIGHT)), velocity(0.f, 0.f) {
	shape.setFillcolor(sf::Color::Green);
	shape.setOrigin(WIDTH/2.f, HEIGHT/2.f);
	shape.setPosition(startX, startY);
}

//input handling
void Player::handleInput() {
	velocity.x = 0.f;
	const bool moveLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	const bool moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPrssed(sf::Keyboard::D);

	if (moveLeft && !moveRight) 
		velocity.x = -MOVE_SPEED;
	else if (moveRight && !moveLeft)
		velocity.x = MOVE_SPEED;
}

//Physics
void Player::update(float deltaTime, float screenWidth) {
	velcoity.y += GRAVITY * deltaTime; //gravity downward acceleration
	sf::Vector2f pos = shape.getPosition();
	pos.x += velocity.x * deltaTime;
	pos.y += velocity.y * deltaTime;
	
	const float halfWidth = WIDTH / 2.f;
	if (pos.x + halfWidth < 0.f)
		pos.x = screenWidth + halfWidth;
	else if (pos.x - halfWidth > screenWidth)
		pos.x = -halfWidth;
	
	shape.setPosition(pos);
}


//rendering
void Player::draw(sf::RenderWindow& window) const {
	window.draw(shape);
} 

//jumping
void Player::jump() { 
	velocity.y = JUMP_SPEED;
}

//Queries
sf::FloatRect Player::getBounds() const {
	return shape.getGlobalBounds();
}
sf::Vector2f Player::getPosition() const {
	return shape.getPosition();
}
float Player::getVelocityY() const {
	return velocity.y;
} 
bool Player::isFalling() const { 
	return velocity.y > 0.f;
}

//Reset
void Player::reset(float startX, float startY) { 
	velocity = sf::Vector2f(0.f, 0.f);
	shape.setPosition(startX, startY);
}
