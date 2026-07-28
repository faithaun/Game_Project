#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>


class Player {
public:
	//construct the player at starting position
	Player(float startX, float startY);
	
	//reads keyboard state and set horizontal velocity
	void handleInput();

	//adds downward acceleration to vertical velocity
	void applyGravity(float deltaTime);

	//sets vertical velocty to upward jump impulse, call playstate when landing is detected
	void jump();
	void bigJump();

	//integrate velocity into position and wrps areound screen edges
	void update(float deltaTime, float windowWidth);

	//draws the player shape to given window
	void draw(sf::RenderWindow& window) const;

	//getter (used by playstate)
	sf::FloatRect getBounds() const;
	sf::Vector2f getPosition() const;
	float	getVelocityY() const;
	int getAmmo() const;

	//setters//mutators
	void setPosition(float x, float y);
	void setVelocityY(float vy);
	void reset(float x, float y);  //full reset, position and zero velocity


	//ammo /shooting
	void addAmmo(int amount);
	bool hasAmmo() const;
	void useAmmo();
	
	//banana stat tracking - never decrease when shooting 
	void recordBananaCollected();
	int getBananaCollected() const;

	//return true exactly once per actual key press (not held-down repeacts) 
	//palystate use this to decide whether to fire
	bool consumeFirePressed();

private:
	sf::RectangleShape shape;  //represent the player
	sf::Vector2f velocity;        //current velocity
	
	int ammo = 0; 
	int bananasCollected = 0;
	bool spaceWasDown = false;
	bool firePressedThisFrame = false;

	static const float WIDTH;
	static const float HEIGHT;
	static const float GRAVITY;
	static const float JUMP_VELOCITY;
	static const float BIG_JUMP_VELOCITY;
	static const float MOVE_SPEED;
};

#endif 

