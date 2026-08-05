#ifndef BEE_NEST_HPP
#define BEE_NEST_HPP

#include "Obstacle.hpp"
#include <cstdlib>

class BeeNest : public Obstacle {
public:
	explicit BeeNest(sf::Vector2f position);

	bool onHitFromBelow(Player& player) override;
	bool onLandedOnTop(Player& player) override;

	// beenest cant be shot by ammo 
	bool isShootable() const override { 
		return false;
	}
	bool bouncesBullets() const override {
		return true;
	}


	void onUpdate(float deltaTime) override;
	
	//playstate check this each frame to know when to spawn bees and remove the nest
	bool isReadyToSpawn() const {
		return readyToSpawn && !alreadySpawned;
	} 
	void markSpawned() {
		alreadySpawned = true;
	}
	int getBeeCount() const {
		return beeCount; 
	} 

	//PlayState class this every fame 
	void setPlayerNear(bool near) {
		if (near) { 
		playerIsNear = near;
		}
	}

private:
	float spawnTimer = 0.5f;  //seconds before bee release
	bool readyToSpawn = false;
	bool alreadySpawned = false;
	bool playerIsNear = false;
	int beeCount = 6 + (std::rand() % 3); //6, 7, 8
};

#endif
