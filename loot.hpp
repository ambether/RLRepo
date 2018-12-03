#pragma once

class Loot {
public:
	bool canCollect(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
	bool collect(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
	virtual bool canUse(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer) = 0;
	virtual void use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
	void drop(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
};

class Healer : public Loot {
public:
	float amt; // Amount of hp restored

	Healer(float amt);
	bool canUse(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
	void use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
};

class DamageSpell : public Loot {
protected:
	float range, dmg;
	DamageSpell(float range, float dmg);
};

class LightningBolt : public DamageSpell {
public:
	LightningBolt(float range, float dmg);
	bool canUse(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
	void use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
};

class Fireball : public DamageSpell {
public:
	float radius;
	Fireball(float range, float dmg, float radius);
	bool canUse(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
	void use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
private:
	int x, y;
};