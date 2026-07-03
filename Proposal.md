# Jungle Jumper 


## Description:
Jungle Jumper is a game inspired by Doodle jump. Play as a monkey that continuously jump up on tree branches and be able to survive as long as possible while avoiding obstacles and collecting power ups. 

## Features:    
- 5 different "stages" - Each with its own challenges 
- Animations - jump, moving left and right, and attack
- Power-up - Collect differemt items for temporary abilities:
	- Banana - used as bullets
	- Shield - invisibility
	- Spring - for faster jump 
- Obstacles - Face off against different challenges depending on the score
	- Broken branch/random moving branch
	- Bird but stationary
	- Falling rocks
	- Bee swarms 
- Save/Load System - Score is save and update score

## Gameplay:
1. The monkey starts on a branch.
2. Player can move the monkey left and right, attack using the keyboard, and must land on branches.
3. Avoid obstacles while climbing.
	- different obstacles for each "stage"
4. Collect power ups to increase survival.
5. Game ends if monkey fall or hits an obstacle.


## Project Structure (Tentative)

```text 
JungleJumper/
|---main    # game loop
|---state   # play(pause), game over, mainmenu(start)
|---player
|---platform   # normal, moving, breakable branches
|---Obstacles
|---Power Ups
|---bullet  
|---Score manager  #keep track of the high score, bananas collected 
|---Resources  #images,fonts, and maybe sounds
``` 



## Design Pattern to be use (Tentative): 
1. Factory Pattern: creates obstacles and powerups
2. Strategy Pattern: each obstacle move differently
3. Singleton Pattern: for resources
4. Object Pool Pattern: bullets
