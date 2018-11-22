#include "main.hpp"

Entity::Entity(int x, int y, const char * name, int ch, const TCODColor & col) 
	: x(x), y(y), 
	name(name), ch(ch), 
	color(col),
	blocks(true),
	combat(nullptr),
	mortal(nullptr),
	ai(nullptr),
	loot(nullptr),
	container(nullptr) {}

void Entity::render() const {
	TCODConsole::root->setChar(x, y, ch);
	TCODConsole::root->setCharForeground(x, y, color);
}

void Entity::update(std::shared_ptr<Entity> owner) { if(ai) ai->update(owner); }

float Entity::getDistance(int cx, int cy) const {
	int dx = x - cx;
	int dy = y - cy;
	return sqrtf(dx*dx + dy*dy);
}
