#pragma once

class Loot {
public:
	bool collect(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> wearer);
	virtual bool use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> wearer);
	void drop(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
};

class Healer : public Loot {
public:
	float amt; // Amount of hp restored

	Healer(float amt);
	bool use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
};

class DamageSpell : public Loot {
protected:
	float range, dmg;
	DamageSpell(float range, float dmg);
};

class LightningBolt : public DamageSpell {
public:
	LightningBolt(float range, float dmg);
	bool use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
};

class Fireball : public DamageSpell {
public:
	Fireball(float range, float dmg);
	bool use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
};