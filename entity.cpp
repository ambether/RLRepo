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

/* bool Entity::moveOrAttack(int x, int y) {
	if(engine.dungeon->isWall(x, y)) return false;
	for(auto & ent : engine.entityList) {
		if(ent->x == x && ent->y == y) {
			std::cout << "The " << ent->name << " memes on ya." << std::endl;
			return false;
		}
	}
	this->x = x; this->y = y;
	return true;
}*/

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
