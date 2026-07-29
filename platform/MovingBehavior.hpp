#ifndef MOVING_BEHAVIOR_HPP
#define MOVING_BEHAVIOR_HPP

#include "PlatformBehavior.hpp"
#include <iostream>

// Slides the platform horizontally between [minX, maxX], reversing
// direction whenever it hits either bound.
class MovingBehavior : public PlatformBehavior {
public:
	MovingBehavior(float minX, float maxX, float speed) : minX(minX), maxX(maxX), speed(speed) {}

   	void update(sf::RectangleShape& shape, float dt) override {
        	sf::Vector2f pos = shape.getPosition();
        	pos.x += speed * direction * dt;

        	if (pos.x <= minX) {
            		pos.x = minX;
            		direction = 1.f;
       		 } else if (pos.x + shape.getSize().x >= maxX) {
            		pos.x = maxX - shape.getSize().x;
            		direction = -1.f;
        	}
        	shape.setPosition(pos);
    	}
	const sf::Texture* getTextureOverride() const override {
		return &getTexture();
	}

private:
	float minX, maxX;
    	float speed;
    	float direction = 1.f;

	static sf::Texture& getTexture() {
		static sf::Texture texture;
		static bool loaded = false;
		if (!loaded) {
			if (!texture.loadFromFile("resources/moving_platform.png")) {
				std::cerr << "Warning: could not load moving platform" << std::endl;
			}
			loaded = true;
		}
		return texture;
	}

};

#endif // MOVING_BEHAVIOR_HPP

