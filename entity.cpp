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

void Entity::update(shared_ptr<Entity> owner) { if(ai) ai->update(owner); }

void Entity::gainEnergy() { if(ai) ai->gainEnergy(); }

float Entity::getDistance(int cx, int cy) const {
	int dx = x - cx;
	int dy = y - cy;
	return sqrtf(dx*dx + dy*dy);
}

shared_ptr<Entity> Entity::clone() const {
	shared_ptr<Entity> cpy = std::make_shared<Entity>();
	cpy->x = x;
	cpy->y = y;
	cpy->name = _strdup(name);
	cpy->ch = ch;
	cpy->color = color;
	cpy->blocks = blocks;

	// ai, loot, and interaction have to be cloned because they are polymorphic
	if(combat)		{ cpy->combat = std::make_shared<Combat>(*combat); }
	if(mortal)		{ cpy->mortal = std::make_shared<Mortal>(*mortal); }
	if(ai)			{ cpy->ai = ai->clone(); }
	if(loot)		{ cpy->loot = loot->clone(); }
	if(container)	{ cpy->container = std::make_shared<Container>(*container); }
	if(interaction) { cpy->interaction = interaction->clone(); }
	if(spellCaster) { cpy->spellCaster = std::make_shared<SpellCaster>(*spellCaster); }

	return cpy;
}
