#ifndef BEE_HPP
#define BEE_HPP

#include "Obstacle.hpp"

class Bee : public Obstacle {
public: 
	Bee(sf::Vector2f position, float angleRadians, float speed);
		
	bool onHitFromBelow(Player& player) override;
	bool onLandedOnTop(Player& player) override;
};

#endif
