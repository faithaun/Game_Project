#ifndef SHIELD_HPP
#define SHIELD_HPP

#include "PowerUp.hpp"

//grants player temporary invincible when collected, doesnt protect player from falling
class Shield : public PowerUp {
public: 
	explicit Shield(sf::Vector2f position);
	
	void apply(Player& player) override;
	void draw(sf::RenderWindow& window) const override;
	//consumedOnUse() use default - disappear after pickup

private: 
	sf::Texture texture;
	sf::Sprite sprite;
	
	static constexpr float SHIELD_DURATION = 6.f;   //seconds of invincibility
};

#endif //SHIELD_HPP
