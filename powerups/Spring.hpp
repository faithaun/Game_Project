#ifndef SPRING_HPP
#define SPRING_HPP

#include "PowerUp.hpp"

// A spring sits on top of a platform and gives the player a stronger
// jump than normal when they land on it. Unlike most power-ups, it's
// reusable -- it never disappears after use.
class Spring : public PowerUp {
public:
	explicit Spring(sf::Vector2f position);

    	void apply(Player& player) override;
    	bool consumedOnUse() const override { 
		return false; 
	} // stays after use

private:
	static sf::Texture& getTexture();
};

#endif // SPRING_HPP


