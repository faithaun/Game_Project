#include "BulletPool.hpp"

BulletPool::BulletPool(int poolSize) : bullets(poolSize) {
	// all bullets start inactive - pool is ready to hand them out via fire w/o allocating any new later
}

void BulletPool::fire(sf::Vector2f position) {
	for (auto& bullet : bullets) {
		if (!bullet.isActive()) {
			bullet.activate(position);
			return;   //reuse the first free slot found
		}
		//all bullets currently on scrren - ignore this shot rather than growing the pool
	}
}


void BulletPool::update(float deltaTime, float topOfViewY) {
	for (auto& bullet : bullets) {
		if (bullet.isActive()) {
			bullet.update(deltaTime);
			if (bullet.getBounds().top + bullet.getBounds().height < topOfViewY) {
				bullet.deactivate();  //scrolled off the top of screen
			}
		}
	}
}

void BulletPool::draw(sf::RenderWindow& window) const {
	for (const auto& bullet : bullets) {
		bullet.draw(window);
	}
}

std::vector<Bullet>& BulletPool::getBullets() {
	return bullets;
}

