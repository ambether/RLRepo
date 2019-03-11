#include "main.hpp"

Container::Container() : size(0) {}

Container::Container(int size) : size(size) {}

Container::Container(const Container & obj) {
	size = obj.size;
	for(auto & e : obj.inventory) {
		inventory.emplace_back(e->clone());
	}
}

Container::~Container() { inventory.clear(); }

bool Container::canAdd(shared_ptr<Entity> entity) {
	if(size > 0 && inventory.size() >= size) return false;
	return true;
}

bool Container::add(shared_ptr<Entity> entity) {
	if(size > 0 && inventory.size() >= size) { return false; }
	inventory.push_back(entity);
	return true;
}

void Container::del(shared_ptr<Entity> entity) {
	auto it = std::find(inventory.begin(), inventory.end(), entity);
	inventory.erase(it);
}
