CXX := g++
CXXFLAGS := -std=c++17 -Wall
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system

TARGET := main

SOURCES :=  main.cpp Game.cpp Player.cpp  state/MainMenuState.cpp state/PlayState.cpp ui/Button.cpp powerups/PowerUp.cpp powerups/Spring.cpp powerups/Banana.cpp powerups/Shield.cpp  obstacles/Obstacle.cpp obstacles/Bird.cpp bullet/Bullet.cpp bullet/BulletPool.cpp
.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET) 

clean: 
	rm -f $(TARGET)

