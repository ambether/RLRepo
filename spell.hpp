#pragma once

class Spell {
public:
	Spell(std::string name, int level, float range, float radius, const TCODColor color);
	Spell(std::string name, int level, float range, const TCODColor color);

	virtual bool canUse(shared_ptr<Entity> owner) = 0;
	virtual void use(shared_ptr<Entity> owner) = 0;

	int getLevel() const;
	float getRange() const;
	float getRadius() const;
	std::string getName() const;
protected:
	int level;
	float range;
	float radius;
	std::string name;
	const TCODColor color;
};


class DamageSpell : public Spell {
public:
	DamageSpell(std::string name, int level, float range, float radius, int damage, const TCODColor color);
	DamageSpell(std::string name, int level, float range, int damage, const TCODColor color);

	bool canUse(shared_ptr<Entity> owner) override;
	void use(shared_ptr<Entity> owner) override;

	int getDamage() const;
protected:
	int damage;
	int tx, ty;
};

