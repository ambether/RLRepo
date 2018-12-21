#pragma once

class Action {
public:
	struct ActionResult {
		std::shared_ptr<Action> alternate;
		ActionResult(std::shared_ptr<Action> alternate=NULL);
	};

	virtual ~Action() {};
	virtual ActionResult execute() = 0;
};


class MoveAction : public Action {
public:
	MoveAction(std::shared_ptr<Entity> owner, int dx, int dy);
	ActionResult execute();
private:
	std::shared_ptr<Entity> owner;
	int dx, dy;
};


class MoveAtPlayerAction : public Action {
public:
	MoveAtPlayerAction(std::shared_ptr<Entity> owner);
	ActionResult execute();
private:
	std::shared_ptr<Entity> owner;
};


class AttackAction : public Action {
public:
	AttackAction(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target);
	ActionResult execute();
private:
	std::shared_ptr<Entity> attacker, target;
}; 


class IdleAction : public Action {
public:
	IdleAction(std::shared_ptr<Entity> owner);
	ActionResult execute();
private:
	std::shared_ptr<Entity> owner;
};


class CollectAction : public Action {
public:
	CollectAction(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
	ActionResult execute();
private:
	std::shared_ptr<Entity> owner, bearer;
};


class UseAction : public Action {
public:
	UseAction(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer);
	ActionResult execute();
private:
	std::shared_ptr<Entity> owner, bearer;
};


class InteractAction : public Action {
public:
	InteractAction(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> interacter);
	ActionResult execute();
private:
	std::shared_ptr<Entity> owner, interacter;
};


class CastAction : public Action {
public:
	CastAction(std::shared_ptr<Spell> spell, std::shared_ptr<Entity> caster);
	ActionResult execute();
private:
	std::shared_ptr<Spell> spell;
	std::shared_ptr<Entity> caster;
};