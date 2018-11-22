#pragma once

class Entity {
public:
	int x, y, ch;
	TCODColor color;
	
	const char * name;

	bool blocks;

	std::shared_ptr<Combat> combat;
	std::shared_ptr<Mortal> mortal;
	std::shared_ptr<Ai> ai;
	std::shared_ptr<Loot> loot;
	std::shared_ptr<Container> container;

	Entity(int x, int y, const char * name, int ch, const TCODColor & col);

	void render() const;
	void update(std::shared_ptr<Entity> owner);
	float getDistance(int cx, int cy) const;
};