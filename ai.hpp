#pragma once

class Ai {
public:
	Ai();
	Ai(int speed);
	virtual shared_ptr<Ai> clone() const = 0;
	virtual void update(shared_ptr<Entity> owner) = 0;
	void gainEnergy();
	void setSpeed(int speed);
	int getSpeed() const;
protected:
	int speed, energy;
	void spendEnergy();
};

class playerAi : public Ai {
public:
	playerAi();
	playerAi(const playerAi & obj); // Copy ctor
	shared_ptr<Ai> clone() const override;
	void update(shared_ptr<Entity> owner);
protected:
	void handleActionKey(shared_ptr<Entity> owner, int ascii);
	bool moveOrAttack(shared_ptr<Entity> owner, int tX, int tY);
	shared_ptr<Entity> chooseFromInv(shared_ptr<Entity> owner);
	shared_ptr<Spell> chooseFromSpells(shared_ptr<Entity> owner);
	TCOD_key_t chooseInteractDirection() const;
};

class mobAi : public Ai {
public:
	mobAi();
	mobAi(int speed);
	mobAi(const mobAi & obj); // Copy ctor
	shared_ptr<Ai> clone() const override;
	void update(shared_ptr<Entity> owner);
protected:
	int moveCount;
	void moveOrAttack(shared_ptr<Entity> owner);
	void idle(shared_ptr<Entity> owner);
};