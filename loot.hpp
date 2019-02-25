#pragma once

class Loot {
public:
	bool canCollect(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	bool collect(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	virtual bool canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) = 0;
	virtual void use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	void drop(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
};

class Healer : public Loot {
public:
	int amt; // Amount of hp restored

	Healer(int amt);
	bool canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	void use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
};

class DamageSpellItem : public Loot {
public:
	DamageSpellItem(float range, int dmg);
protected:
	float range;
	int dmg;
};

class LightningBolt : public DamageSpellItem {
public:
	LightningBolt(float range, int dmg);
	bool canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	void use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
};

class Fireball : public DamageSpellItem {
public:
	Fireball(float range, int dmg, float radius);
	bool canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	void use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
private:
	int x, y;
	float radius;
};