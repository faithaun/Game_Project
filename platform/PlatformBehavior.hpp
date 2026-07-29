#ifndef PLATFORM_BEHAVIOR_HPP
#define PLATFORM_BEHAVIOR_HPP

#include <SFML/Graphics.hpp>

// Strategy pattern (mirrors MovementStrategy used by Obstacle): defines
// how a platform behaves each frame and what happens the moment the
// player lands on it. Platform holds one of these via composition.
class PlatformBehavior {
public:
	virtual ~PlatformBehavior() = default;

	// Called every frame, whether or not the player is on the platform.
	virtual void update(sf::RectangleShape& shape, float dt) {}

    	// Called once, the frame the player lands on top.
    	virtual void onLanded() {}

    	// Whether the platform can currently be landed on / collided with.
    	virtual bool isSolid() const {
		 return true; 
	 }	

    	//overide this to show own imaqge 
 	virtual const sf::Texture* getTextureOverride() const {
		return nullptr;    //use normal platform texture
	}  
};

// Default: plain platform, identical to current behavior.
class NormalBehavior : public PlatformBehavior {
    // uses all base defaults — nothing to override
};

#endif // PLATFORM_BEHAVIOR_HPP

