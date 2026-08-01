#ifndef POWERUP_HPP
#define POWERUP_HPP

#include <SFML/Graphics.hpp>

class Player; // forward declare -- avoids needing Player.hpp here


// PowerUp (abstract base class)
// Every power-up (Spring, Shield, Banana, ...) inherits from this.
// PlayState checks collision against the shared bounds/position,
// then calls apply() to let each power-up do its own unique effect.

class PowerUp {
public:
	explicit PowerUp(sf::Vector2f position);
    	virtual ~PowerUp() = default;

    	// The actual effect on the player -- each subclass defines its own.
    	virtual void apply(Player& player) = 0;

    	// Most power-ups (Shield, Banana) disappear after one use.
    	// Spring overrides this to stay reusable, since real platforms
    	// keep their springs after bouncing off them.
    	virtual bool consumedOnUse() const { return true; }

    	sf::FloatRect getBounds() const;
    	virtual void draw(sf::RenderWindow& window) const;

protected:
    	// Subclasses configure size/color/outline in their own constructor.
    	sf::RectangleShape shape;
};

#endif // POWERUP_HPP

