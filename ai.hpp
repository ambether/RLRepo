#pragma once

class Ai {
public:
	virtual void update(std::shared_ptr<Entity> owner) = 0;
};

class playerAi : public Ai {
public:
	void update(std::shared_ptr<Entity> owner);
protected:
	void handleActionKey(std::shared_ptr<Entity> owner, int ascii);
	bool moveOrAttack(std::shared_ptr<Entity> owner, int tX, int tY);
	std::shared_ptr<Entity> chooseFromInv(std::shared_ptr<Entity> owner);
};

class mobAi : public Ai {
public:
	void update(std::shared_ptr<Entity> owner);
protected:
	int moveCount;
	void moveOrAttack(std::shared_ptr<Entity> owner, int tX, int tY);
};