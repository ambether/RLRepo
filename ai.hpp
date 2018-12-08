#pragma once

class Ai {
public:
	Ai(int speed);
	virtual void update(std::shared_ptr<Entity> owner) = 0;
	void gainEnergy();
protected:
	int speed, energy;
	void spendEnergy();
};

class playerAi : public Ai {
public:
	playerAi();
	void update(std::shared_ptr<Entity> owner);
protected:
	void handleActionKey(std::shared_ptr<Entity> owner, int ascii);
	bool moveOrAttack(std::shared_ptr<Entity> owner, int tX, int tY);
	std::shared_ptr<Entity> chooseFromInv(std::shared_ptr<Entity> owner);
	TCOD_key_t chooseInteractDirection() const;
};

class mobAi : public Ai {
public:
	mobAi(int speed);
	void update(std::shared_ptr<Entity> owner);
protected:
	int moveCount;
	void moveOrAttack(std::shared_ptr<Entity> owner);
	void idle(std::shared_ptr<Entity> owner);
};