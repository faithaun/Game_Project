#pragma once
#include <SFML/Graphics.hpp>

class Player {
public: 
	static constexpr float WIDTH = 40.f;
	static constexpr float HEIGHT = 40.f;

	void handleInput();

	void update(float deltaTime, float screenWidth);

	void draw(sf::RenderWindow& window) const;

	void jump();

	sf::FloatRect getBounds() const;
	sf::Vector2f getPosition() const;
	float getVelocityY() const;
	bool isFalling() const;
	void reset(float startX, float startY);

private:
	sf::RectangleShape shape;
	sf::Vector2f	velocity;
	
	static constexpr float MOVE_SPEED = 320.f;   //horizontal speed
	static constexpr float GRAVITY = 1000.f;   //downward acceleration
	static constexpr float JUMP_SPEED = -650.f;        //instantenous jump velocity
};
