#ifndef  PLATFORM_HPP
#define PLATFORM_HPP 


#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "../powerups/PowerUp.hpp"
#include "PlatformBehavior.hpp"

//single platform, use a shared image of a plain colored rectangle
struct Platform {     //why use struct?
	sf::RectangleShape shape;	// used for position/size/collision
	std::unique_ptr<PowerUp> attachedPowerUp;   //nullptr if its platform has none
	std::unique_ptr<PlatformBehavior> behavior = std::make_unique<NormalBehavior>();

	//advance moving/breaking logic
	void update(float dt) {
		behavior->update(shape, dt);
	}

	//draw platform image and its power up if it has one
	void draw(sf::RenderWindow& window) const {
		sf::Sprite sprite;
		
		const sf::Texture* override = behavior->getTextureOverride();
		const sf::Texture& textureToUse = override ? *override : getTexture();
		sprite.setTexture(textureToUse);

		sf::Vector2u texSize = getTexture().getSize();
		sf::Vector2f shapeSize = shape.getSize();
		if (texSize.x > 0 && texSize.y > 0) {
			sprite.setScale(shapeSize.x / texSize.x, shapeSize.y / texSize.y);
		}

		sprite.setPosition(shape.getPosition());
		window.draw(sprite);

		if (attachedPowerUp) {
			attachedPowerUp->draw(window);
		} 
	}

	bool checklanding(const sf::FloatRect& playerBounds, float previousPlayerBottom) const {
		if (!behavior->isSolid()) 
			return false;   //broken platforms cant be landed

		sf::FloatRect platformBounds = shape.getGlobalBounds();
		float currentBottom = playerBounds.top + playerBounds.height;
		
		bool horizontalOverlap = 
			playerBounds.left + playerBounds.width > platformBounds.left &&
			playerBounds.left < platformBounds.left + platformBounds.width;
	
		bool crossedTopThisFrame = 
			previousPlayerBottom <= platformBounds.top &&
			currentBottom >= platformBounds.top;
	
		return horizontalOverlap && crossedTopThisFrame;
	}


	//call right after checklanding returns true 
	void notifyLanded() {
		behavior->onLanded();
	}

	float getTop() const {
		return shape.getGlobalBounds().top;
	}

private:
	//single shared texture, loaded from disk only first time any platform needs it 
	// every platfrom after reuses the same already loaded image instead of reloading file repeat
	static sf::Texture& getTexture() {
		static sf::Texture texture;
		static bool loaded = false;
		if (!loaded) {
			if (!texture.loadFromFile("resources/branch.png")) {
				std::cerr << "Warnin: could not load platform.png" << std::endl;
			}
			loaded = true;
		} 
		return texture;
	}
};

#endif //PLATFORM_HPP
