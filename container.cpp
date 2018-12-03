#include "main.hpp"

Container::Container(int size) : size(size) {}

Container::~Container() { inventory.clear(); }

bool Container::canAdd(std::shared_ptr<Entity> entity) {
	if(size > 0 && inventory.size() >= size) return false;
	return true;
}

bool Container::add(std::shared_ptr<Entity> entity) {
	if(size > 0 && inventory.size() >= size) { return false; }
	inventory.push_back(entity);
	return true;
}

void Container::del(std::shared_ptr<Entity> entity) {
	auto it = std::find(inventory.begin(), inventory.end(), entity);
	inventory.erase(it);
}
