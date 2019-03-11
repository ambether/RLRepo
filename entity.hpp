#pragma once

class Entity {
public:
	int x, y, ch;
	TCODColor color;
	const char * name;
	bool blocks;

	shared_ptr<Combat> combat;
	shared_ptr<Mortal> mortal;
	shared_ptr<Ai> ai;
	shared_ptr<Loot> loot;
	shared_ptr<Container> container;
	shared_ptr<Interaction> interaction;
	shared_ptr<SpellCaster> spellCaster;

	Entity();
	Entity(int x, int y, const char * name, int ch, const TCODColor & col);

	void render(TCODConsole * renderConsole) const;
	void update(shared_ptr<Entity> owner);
	void gainEnergy();

	float getDistance(int cx, int cy) const;

	shared_ptr<Entity> clone() const;
};