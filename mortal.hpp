#pragma once

class Mortal {
public:
	float maxHp, hp, def;

	const char * corpseName;

	Mortal(float maxHp, float def, const char * corpseName);

	inline bool isDead() { return hp <= 0; }

	float takeDamage(std::shared_ptr<Entity> owner, float dmg);

	virtual void die(std::shared_ptr<Entity> owner);

	float heal(float amount);
};

class pcMortal : public Mortal {
public:
	pcMortal(float maxHp, float def, const char * corpseName);

	void die(std::shared_ptr<Entity> owner);
};

class npcMortal : public Mortal {
public:
	npcMortal(float maxHp, float def, const char * corpseName);

	void die(std::shared_ptr<Entity> owner);
};