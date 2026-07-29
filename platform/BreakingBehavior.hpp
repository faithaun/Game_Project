#ifndef BREAKING_BEHAVIOR_HPP
#define BREAKING_BEHAVIOR_HPP

#include "PlatformBehavior.hpp"
#include <iostream>

// Player gets one normal bounce, then the platform flashes briefly
// and gives way — it stops being solid and fades out.
class BreakingBehavior : public PlatformBehavior {
public:
    	void update(sf::RectangleShape& shape, float dt) override {
        	if (triggered) {
            		fallSpeed += 900.f * dt;     //gravity - speeds up as it falls
			shape.move(0.f, fallSpeed * dt);
           		 
			fadeTimer += dt;
			float alpha = 255.f * (1.f  - (fadeTimer / fadeDuration));
			if (alpha < 0.f) alpha = 0.f;
			shape.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
			
			if (fadeTimer >= fadeDuration) {
				fullyGone = true;
           		 }
        	} 
	}

	void onLanded() override {
		triggered = true;  //starts falling/fading immediately on contact
	}

	bool isSolid() const override {
		return !triggered;
	}
	
	
	const sf::Texture* getTextureOverride() const override {
		return &getCrackedTexture();    //cracked while coundown runs 
	    
	} 
	
	bool isGone() const {
		return fullyGone; 
	}


private:
	bool triggered = false;
	bool fullyGone = false;
	float fallSpeed = 0.f;
	float fadeTimer = 0.f;
	float fadeDuration = 0.6f;

	static sf::Texture& getCrackedTexture() {
		static sf::Texture texture;
		static bool loaded = false;
		if (!loaded) { 
			if (!texture.loadFromFile("resources/broken_platform.png")) {
				std::cerr << "Warning: could not load broken platform" << std::endl;
			} 
			loaded = true;
		}
		return texture;
	}
};

#endif // BREAKING_BEHAVIOR_HPP

