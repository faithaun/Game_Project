#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP


#include <SFML/Graphics.hpp>
#include <memory>
#include "MovementStrategy.hpp"

class Player;    //forward declare to avoid needing player.hpp

class Obstacle {
public: 
	Obstacle(sf::Vector2f position, sf::Vector2f size, sf::Color color, std::unique_ptr<MovementStrategy> strategy);
	virtual ~Obstacle() = default;

	void update(float deltaTime);	 //delegates to movement strategy
	
	//player touch obstacle from below 
	virtual bool onHitFromBelow(Player& player) = 0;

	//player fell and landed cleanly top of obstacle
	virtual void onLandedOnTop(Player& player) = 0;

	sf::FloatRect getBounds() const;
	void draw (sf::RenderWindow& window) const;

protected:
	sf::RectangleShape shape;

private:
	std::unique_ptr<MovementStrategy> movementStrategy;
};


#endif //OBSTACLE_HPP
