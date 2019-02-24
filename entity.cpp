#include "main.hpp"

Entity::Entity() :
	x(0), y(0),
	name(""), ch(' '),
	color(TCODColor::white),
	blocks(false),
	combat(nullptr),
	mortal(nullptr),
	ai(nullptr),
	loot(nullptr),
	container(nullptr),
	interaction(nullptr),
	spellCaster(nullptr) {}

Entity::Entity(int x, int y, const char * name, int ch, const TCODColor & col)
	: x(x), y(y), 
	name(name), ch(ch), 
	color(col),
	blocks(true),
	combat(nullptr),
	mortal(nullptr),
	ai(nullptr),
	loot(nullptr),
	container(nullptr),
	interaction(nullptr),
	spellCaster(nullptr) {}

void Entity::render(TCODConsole * renderConsole) const {
	renderConsole->setChar(x, y, ch);
	renderConsole->setCharForeground(x, y, color);
}

void Entity::update(std::shared_ptr<Entity> owner) { if(ai) ai->update(owner); }

void Entity::gainEnergy() { if(ai) ai->gainEnergy(); }

float Entity::getDistance(int cx, int cy) const {
	int dx = x - cx;
	int dy = y - cy;
	return sqrtf(dx*dx + dy*dy);
}
