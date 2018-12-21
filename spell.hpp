#pragma once

class Spell {
public:
	Spell(std::string name, int level, float range, float radius, const TCODColor color);
	Spell(std::string name, int level, float range, const TCODColor color);

	virtual bool canUse(std::shared_ptr<Entity> owner) = 0;
	virtual void use(std::shared_ptr<Entity> owner) = 0;

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
	DamageSpell(std::string name, int level, float range, float radius, float damage, const TCODColor color);
	DamageSpell(std::string name, int level, float range, float damage, const TCODColor color);

	bool canUse(std::shared_ptr<Entity> owner) override;
	void use(std::shared_ptr<Entity> owner) override;

	float getDamage() const;
protected:
	float damage;
	int tx, ty;
};

