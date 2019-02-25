#pragma once

class Action {
public:
	struct ActionResult {
		shared_ptr<Action> alternate;
		ActionResult(shared_ptr<Action> alternate=NULL);
	};

	virtual ~Action() {};
	virtual ActionResult execute() = 0;
};


class MoveAction : public Action {
public:
	MoveAction(shared_ptr<Entity> owner, int dx, int dy);
	ActionResult execute();
private:
	shared_ptr<Entity> owner;
	int dx, dy;
};


class MoveAtPlayerAction : public Action {
public:
	MoveAtPlayerAction(shared_ptr<Entity> owner);
	ActionResult execute();
private:
	shared_ptr<Entity> owner;
};


class AttackAction : public Action {
public:
	AttackAction(shared_ptr<Entity> attacker, shared_ptr<Entity> target);
	ActionResult execute();
private:
	shared_ptr<Entity> attacker, target;
}; 


class IdleAction : public Action {
public:
	IdleAction(shared_ptr<Entity> owner);
	ActionResult execute();
private:
	shared_ptr<Entity> owner;
};


class CollectAction : public Action {
public:
	CollectAction(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	ActionResult execute();
private:
	shared_ptr<Entity> owner, bearer;
};


class UseAction : public Action {
public:
	UseAction(shared_ptr<Entity> owner, shared_ptr<Entity> bearer);
	ActionResult execute();
private:
	shared_ptr<Entity> owner, bearer;
};


class InteractAction : public Action {
public:
	InteractAction(shared_ptr<Entity> owner, shared_ptr<Entity> interacter);
	ActionResult execute();
private:
	shared_ptr<Entity> owner, interacter;
};


class CastAction : public Action {
public:
	CastAction(shared_ptr<Spell> spell, shared_ptr<Entity> caster);
	ActionResult execute();
private:
	shared_ptr<Spell> spell;
	shared_ptr<Entity> caster;
};