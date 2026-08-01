#ifndef ANGLED_SWARM_MOVEMENT_HPP
#define ANGLED_SWARM_MOVEMENT_HPP

#include "MovementStrategy.hpp"
#include <cmath>

//flies outward in fized random direction at fixed random speed, woobl layered on top for zigzag
class AngledSwarmMovement : public MovementStrategy {
public: 
	AngledSwarmMovement(float angleRadians, float speed, float wobbleAmplitude, float wobbleSpeed) 
		: dirX(std::cos(angleRadians)), dirY(std::sin(angleRadians)),
		speed(speed), wobbleAmplitude(wobbleAmplitude), wobbleSpeed(wobbleSpeed) {
	}
	
	void move(sf::RectangleShape& shape, float deltaTime) override {
		elapsedTime += deltaTime;

		float perpX = -dirY;
		float perpY = dirX;
		float wobble = std::sin(elapsedTime * wobbleSpeed) * wobbleAmplitude;

		sf::Vector2f pos = shape.getPosition();
		float newX = pos.x + (dirX * speed + perpX * wobble) * deltaTime;
		float newY = pos.y + (dirY * speed + perpY * wobble) * deltaTime;
		shape.setPosition(newX, newY);
	}

private: 
	float dirX, dirY;
	float speed;
	float wobbleAmplitude;
	float wobbleSpeed;
	float elapsedTime = 0.f;
};

#endif
