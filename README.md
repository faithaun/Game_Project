# Jungle Jumper  

## About
Jungle Jumper is a 2D infinite vertical climbing platformer built in C++ with SFML graphics Library. Play as a monkey 
bouncing upward across jungle platforms, dodging obstacles, collecting power-ups, and chasing the highest score 
you can reach before you fall. 

## Features
- **
- **Three Platform Behaviors** - Normal, Moving and Broken platfroms, unclock score increases.
- **Power-Up System** - collect and use certain item for temporary abilities:
	- Banana:
	- Shield:
	- Spring: 
- **Obstacle Variety** - avoid and face off against different obstacles including birds, bees, and falling rocks.
- 


## Controls
|    Key    |    Action    |
|-----------| ------------ |
| A/Left Arrow | Move Left |
| D/Right Arrow | Move Right |
| Space | Fire projectile (ammo) |


## Requirements
- C++17 compiler 
- SFML 2.6 

## Project Structure

```text
JungleJumper/
|---main 
|---Game.hpp/.cpp  
|---Player.hpp/.cpp
|---state/
|    |---GameState.hpp
|    |---MainMenuState.hpp/.cpp
|    |---PlayState.hpp/.cpp
|---platform/
|    |---BreakingBehavior.hpp/MovingBehavior.hpp/PlatformBehavior.hpp
|    |---Platform.hpp/.cpp
|---powerups/
|    |---PowerUp.hpp/.cpp
|    |---Banana.hpp/.cpp
|    |---Shield.hpp/.cpp
|    |---Spring.hpp/.cpp
|---obstacles/
|    |---Obstalce.hpp/.cpp 
|    |---FallMovement.hpp/StaticMovement.hpp/MovementStrategy.hpp/AngledSwaemMovement.hpp
|    |---Bee.hpp/.cpp 
|    |---BeeNest.hpp/.cpp
|    |---Bird.hpp/.cpp
|    |---FallingRock.hpp/.cpp  
|---bullet/
|    |---Bullet.hpp/.cpp
|    |---BulletPool.hpp/.cpp
|---resource/      #fonts, graphics(png) 
|---ui/
|    |---Button.hpp/.cpp
```


## How to run: 
### Using Makefile
```text

### Gameplay 
1. Climb high by bouncing across platforms --normal, moving, and broken.
2. Falling off the screen ends the run.
3. Watch out for birds, falling rocks, and bee swarms as your score gets higher.
4. Collect bananas for ammo, springs for a boost, and shield for protection. 
5. Try to climb higher than 









