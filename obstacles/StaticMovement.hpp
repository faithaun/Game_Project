#ifndef STATIC_MOVEMENT_HPP
#define STATIC_MOVEMENT_HPP

#include "MovementStrategy.hpp"

//use by bird - doenst move at all

class StaticMovement : public MovementStrategy {
public: 
	void move(sf::RectangleShape& shape, float deltaTime) override {
	//intentionally empty
	} 
};

#endif //STATIC_MOVEMENT_HPP 
