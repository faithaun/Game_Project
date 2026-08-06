#ifndef OBSTACLE_FACTORY_HPP
#define OBSTACLE_FACTORY_HPP

#include <memory>
#include <SFML/Graphics.hpp>
#include "Obstacle.hpp"

//Factory pattern: centeralize obstacle creation in one place, so playstate doesn't need to know the constructor details of every obstacle 

class ObstacleFactory {
public: 
	static std::unique_ptr<Obstacle> createBird(sf::Vector2f position);
	static std::unique_ptr<Obstacle> createFallingRock(sf::Vector2f position, float fallSpeed);
	static std::unique_ptr<Obstacle> createBee(sf::Vector2f position, float angleRadians, float speed);
	static std::unique_ptr<Obstacle> createBeeNest(sf::Vector2f position);

};

#endif
