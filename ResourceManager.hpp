#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <iostream>

//singleton patten: one shared texture for the whole game 
class ResourceManager { 
public: 
	//delete copy/move 
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	static ResourceManager& getInstance() {
		static ResourceManager instance;  //created once on first use
		return instance;
	}

	sf::Texture& getTexture(const std::string& path) {
		auto it = textureCache.find(path);
		if (it != textureCache.end()) {
			return it->second;  //
		}
		
		sf::Texture texture;
		if (!texture.loadFromFile(path)) {
			std::cerr << "ResourceManager: failed to load " << path << std::endl;
		}
		auto result = textureCache.emplace(path, std::move(texture));
		return result.first->second;
	}


private:
	ResourceManager() = default; //private constructor, only get instance() can create
	std::unordered_map<std::string, sf::Texture> textureCache;
};
