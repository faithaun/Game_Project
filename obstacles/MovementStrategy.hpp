#ifndef MOVEMENT_STRATEGY_HPP
#define MOVEMENT_STRATEGY_HPP

#include <SFML/Graphics.hpp>


// each obstacle owns one to define how it looks


class MovementStrategy {
public: 
	virtual ~MovementStrategy() = default;
	virtual void move (sf::RectangleShape& shape, float deltaTime) = 0;
};

#endif //MOVEMENT_STRATEGY_HPP
