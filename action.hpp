#pragma once

class Action {
public:
	virtual ~Action() {};
	virtual void execute() = 0;
};


class MoveAction : public Action {
public:
	MoveAction(std::shared_ptr<Entity> owner, int tx, int ty);
	void execute();
private:
	std::shared_ptr<Entity> owner;
	int tx, ty;
};


class AttackAction :public Action {
public:
	AttackAction(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target);
	void execute();
private:
	std::shared_ptr<Entity> attacker, target;
};