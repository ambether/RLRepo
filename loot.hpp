#pragma once

class Loot {
public:
	virtual shared_ptr<Loot> clone() const = 0;
	bool canCollect(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	bool collect(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	virtual bool canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) = 0;
	virtual void use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	void drop(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
};

class Healer : public Loot {
public:
	Healer(int amt);
	Healer(const Healer & obj); // Copy ctor
	shared_ptr<Loot> clone() const override;
	bool canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	void use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
private:
	int amt; // Amount of hp restored
};

class DamageSpellItem : public Loot { // Eventually rewrite for more generic scrolls; i.e. flags for targeting type.
public:
	DamageSpellItem(float range, int dmg, float radius = 0.0);
	DamageSpellItem(const DamageSpellItem & obj); // Copy ctor
protected:
	float range;
	int dmg;
	float radius;
};

class LightningBolt : public DamageSpellItem {
public:
	LightningBolt(float range, int dmg);
	shared_ptr<Loot> clone() const override;
	bool canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	void use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
};

class Fireball : public DamageSpellItem {
public:
	Fireball(float range, int dmg, float radius);
	Fireball(const Fireball & obj); // Copy ctor
	shared_ptr<Loot> clone() const override;
	bool canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	void use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
private:
	int x, y;
};