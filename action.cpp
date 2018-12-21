#include "main.hpp"

Action::ActionResult::ActionResult(std::shared_ptr<Action> alternate) : alternate(alternate) {}


MoveAction::MoveAction(std::shared_ptr<Entity> owner, int dx, int dy) : owner(owner), dx(dx), dy(dy) {}

Action::ActionResult MoveAction::execute() {
	for(auto & ent : engine.entityList) {
		if(ent->x == owner->x + dx && ent->y == owner->y + dy) {
			if(ent->interaction && ent->blocks) { return ActionResult(std::make_shared<InteractAction>(ent, owner)); }
			else if(ent->mortal) {
				if(!ent->mortal->isDead()) {
					if(owner != engine.player && ent != engine.player) return ActionResult(std::make_shared<IdleAction>(owner));
					return ActionResult(std::make_shared<AttackAction>(owner, ent));
				}
				else { engine.gui->message(TCODColor::white, "There is a %s here.", ent->name); }
			}
			else { engine.gui->message(TCODColor::white, "There is a %s here.", ent->name); }
		}
	}
	if(owner == engine.player) engine.setComputeFov(true);
	owner->x += dx; owner->y += dy;
	return ActionResult();
}


AttackAction::AttackAction(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target) : attacker(attacker), target(target) {}

Action::ActionResult AttackAction::execute() { 
	attacker->combat->attack(attacker, target); 
	return ActionResult(); 
}


IdleAction::IdleAction(std::shared_ptr<Entity> owner) : owner(owner) {}

Action::ActionResult IdleAction::execute() { return ActionResult(); }


MoveAtPlayerAction::MoveAtPlayerAction(std::shared_ptr<Entity> owner) : owner(owner) {}

Action::ActionResult MoveAtPlayerAction::execute() {
	int dx = engine.player->x - owner->x,
		dy = engine.player->y - owner->y,
		sdx = (dx > 0 ? 1 : -1),
		sdy = (dy > 0 ? 1 : -1),
		distance((int)sqrt(dx*dx + dy*dy));
	if(distance >= 2) {
		dx = (int)(round(dx / distance));
		dy = (int)(round(dy / distance));
		if(engine.dungeon->canWalk(owner->x + dx, owner->y + dy)) {
			return ActionResult(std::make_shared<MoveAction>(owner, dx, dy));
		}
		else if(engine.dungeon->canWalk(owner->x + sdx, owner->y)) {
			return ActionResult(std::make_shared<MoveAction>(owner, sdx, 0));
		}
		else if(engine.dungeon->canWalk(owner->x, owner->y + sdy)) {
			return ActionResult(std::make_shared<MoveAction>(owner, 0, sdy));
		}
	}
	else if(distance == 1 && owner->combat) { return ActionResult(std::make_shared<AttackAction>(owner, engine.player)); }
	else return ActionResult(); // This shouldn't happen but is included just in case
}


CollectAction::CollectAction(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer) : owner(owner), bearer(bearer) {}

Action::ActionResult CollectAction::execute() {
	if(owner->loot->collect(owner, bearer)) { engine.gui->message(TCODColor::lightGrey, "%s collects the %s.", bearer->name, owner->name); }
	return ActionResult();
}


UseAction::UseAction(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer) : owner(owner), bearer(bearer) {}

Action::ActionResult UseAction::execute() {
	owner->loot->use(owner, bearer);
	engine.gui->message(TCODColor::yellow, "%s uses the %s.", bearer->name, owner->name);
	return ActionResult();
}


InteractAction::InteractAction(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> interacter)
	: owner(owner), 
	interacter(interacter) {}

Action::ActionResult InteractAction::execute() {
	engine.gui->message(TCODColor::silver, "%s %s the %s.", interacter->name, owner->interaction->getInteractVerb(), owner->name);
	owner->interaction->interact(owner, interacter);
	return ActionResult();
}


CastAction::CastAction(std::shared_ptr<Spell> spell, std::shared_ptr<Entity> caster) : spell(spell), caster(caster) {}

Action::ActionResult CastAction::execute() {
	engine.gui->message(TCODColor::lightPurple, "%s casts %s.", caster->name, spell->getName().c_str());
	spell->use(caster);
	return ActionResult();
}
