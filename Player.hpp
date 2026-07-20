#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>


class Player {
public:
	Player(float startX, float startY);
	void handleInput();
	void applyGravity(float deltaTime);
	void jump();
	void bigJump();
	void update(float deltaTime, float windowWidth);
	void draw(sf::RenderWindow& window) const;

	//getter
	sf::FloatRect getBounds() const;
	sf::Vector2f getPosition() const;
	float	getVelocityY() const;

	//setters//mutators
	void setPosition(float x, float y);
	void setVelocityY(float vy);
	void reset(float x, float y);

private:
	sf::RectangleShape shape;
	sf::Vector2f velocity;
	
	static const float WIDTH;
	static const float HEIGHT;
	static const float GRAVITY;
	static const float JUMP_VELOCITY;
	static const float BIG_JUMP_VELOCITY;
	static const float MOVE_SPEED;
};

#endif 

