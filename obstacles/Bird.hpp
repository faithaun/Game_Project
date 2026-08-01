#ifndef BIRD_HPP
#define BIRD_HPP

#include "Obstacle.hpp"

//stationary bird - doesn't move, but knocks player if touched
class Bird : public Obstacle {
public:
	explicit Bird(sf::Vector2f position);
	
	bool onHitFromBelow(Player& player) override;
	bool  onLandedOnTop(Player& player) override;
};

#endif //BIRD_HPP
