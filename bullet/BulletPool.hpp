#ifndef BULLET_POOL_HPP
#define BULLET_POOL_HPP

#include "Bullet.hpp"
#include <vector>

//Object pool pattern:
	// pre-allocates a fix set of bullet objects up front and reuses every time the player fires, instead of calling 
	// new/delete on every shot 

class BulletPool {
public: 
	explicit BulletPool(int poolSize = 20);
	
	//activate one currently-inactive bullet, if any are available, doesnt do anything if pool is currently in use
	void fire(sf::Vector2f position);

	//moves all active bullet and deactivate any that have scrolled above the top of visible camera view
	void update(float deltaTime, float topOfViewY);
	
	void draw(sf::RenderWindow& window) const;
	
	//exposed so playstate can check bullet vs. obstacle collisions
	std::vector<Bullet>& getBullets();

private:
	std::vector<Bullet> bullets;  //fixed size after construction
};

#endif //BULLET_POOL_HPP
