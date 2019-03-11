#pragma once

class Mortal {
public:
	int maxHp, hp;
	const char * corpseName;

	Mortal();
	Mortal(int maxHp, const char * corpseName);
	Mortal(const Mortal & obj); // Copy ctor

	virtual shared_ptr<Mortal> clone() const = 0;

	inline bool isDead() { return hp <= 0; }

	int takeDamage(shared_ptr<Entity> owner, int dmg);
	int predictHeal(int amount);
	int heal(int amount);

	virtual void die(shared_ptr<Entity> owner);
};

class pcMortal : public Mortal {
public:
	pcMortal();
	pcMortal(int maxHp, const char * corpseName);
	shared_ptr<Mortal> clone() const override;
	void die(shared_ptr<Entity> owner);
};

class npcMortal : public Mortal {
public:
	npcMortal();
	npcMortal(int maxHp, const char * corpseName);
	shared_ptr<Mortal> clone() const override;
	void die(shared_ptr<Entity> owner);
};