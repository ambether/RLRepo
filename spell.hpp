#pragma once

class Spell {
public:
	const char * name;
	int level;
	float range;
	float radius;
	TCODColor color;
	enum TargetType { SELF, CLOSEST, RANGED } targetType;

	Spell();
	Spell(const Spell & obj); // Copy ctor

	bool canUse(shared_ptr<Entity> owner);
	void use(shared_ptr<Entity> owner);

	void initDamageDealer();
	void initHealer();
	void addDamage(int damage);
	void addHeal(int heal);
private:
	int tx, ty;

	struct _DamageDealer { int damage; } * _damageDealer;
	struct _Healer { int amount; } * _healer;
};