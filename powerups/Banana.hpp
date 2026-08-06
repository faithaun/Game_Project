#ifndef BANANA_HPP
#define BANANA_HPP

#include "PowerUp.hpp"

//Grants the player fixed number of bullet when collected
class Banana : public PowerUp {
public: 
	explicit Banana(sf::Vector2f position);
	void apply(Player& player) override;
	void draw(sf::RenderWindow& window) const override;
//consumedOnUse() uses the default true and disappeard after pickup
private:
	sf::Sprite sprite;

};


#endif //BANANA_HPP
