CXX := g++
CXXFLAGS := -std=c++17 -Wall
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system

TARGET := main

SOURCES :=  main.cpp Game.cpp state/MainMenuState.cpp state/PlayState.cpp ui/Button.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET) 

clean: 
	rm -f $(TARGET)

