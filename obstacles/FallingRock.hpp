#ifndef FALLING_ROCK_HPP
#define FALLING_ROCK_HPP

#include "Obstacle.hpp"

class FallingRock : public Obstacle {
public: 
	FallingRock(sf::Vector2f position, float fallSpeed);
	
	bool onHitFromBelow(Player& player) override;
	bool onLandedOnTop(Player& player) override;
	bool isShootable() const override;
};

#endif
