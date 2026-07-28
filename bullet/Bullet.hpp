#ifndef BULLET_HPP
#define BULLET_HPP

#include <SFML/Graphics.hpp>

// a single banana bullet owned by BulletPool - activated/deactivated and reused
class Bullet {
public:
	Bullet();	

	void activate(sf::Vector2f position);
	void update(float deltaTime);
	void deactivate();

	bool isActive() const;
	sf::FloatRect getBounds() const;
	void draw(sf::RenderWindow& window) const;

private:
	sf::RectangleShape shape;
	bool active = false;

	static constexpr float SPEED = 500.f;  //pixels per second, moving upward
};

#endif //BULLET_HPP
