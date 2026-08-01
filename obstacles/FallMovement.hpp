#ifndef FALL_MOVEMENT_HPP
#define FALL_MOVEMENT_HPP

#include "MovementStrategy.hpp"


//moves obstacle straight down at fixed speed but own random speed and set once spawn time
class FallMovement : public MovementStrategy {
public: 
	explicit FallMovement(float speed) : speed(speed) {
	
	}

	void move(sf::RectangleShape& shape, float deltaTime) override {
		shape.move(0.f, speed * deltaTime);
	}

private:
	float speed;
};

#endif
