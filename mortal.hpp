#pragma once

class Mortal {
public:
	int maxHp, hp;
	const char * corpseName;

	Mortal();
	Mortal(int maxHp, const char * corpseName);

	inline bool isDead() { return hp <= 0; }

	int takeDamage(std::shared_ptr<Entity> owner, int dmg);
	int predictHeal(int amount);
	int heal(int amount);

	virtual void die(std::shared_ptr<Entity> owner);
};

class pcMortal : public Mortal {
public:
	pcMortal();
	pcMortal(int maxHp, const char * corpseName);
	void die(std::shared_ptr<Entity> owner);
};

class npcMortal : public Mortal {
public:
	npcMortal();
	npcMortal(int maxHp, const char * corpseName);
	void die(std::shared_ptr<Entity> owner);
};