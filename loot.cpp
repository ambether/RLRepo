#include "main.hpp"

// Probably want to change a lot of the "use" methods. Seems wrong to send messages to gui from here.

bool Loot::collect(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer) {
	if(bearer->container && bearer->container->add(owner)) {
		auto it = std::find(engine.entityList.begin(), engine.entityList.end(), owner);
		engine.entityList.erase(it);
		it = std::find(engine.inactiveEntities.begin(), engine.inactiveEntities.end(), owner);
		engine.inactiveEntities.erase(it);
		return true;
	}
	return false;
}

bool Loot::use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer) {
	if(bearer->container) {
		bearer->container->del(owner);
		return true;
	}
	return false;
}

void Loot::drop(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer) {
	if(bearer->container) {
		bearer->container->del(owner);
		engine.entityList.push_back(owner); engine.inactiveEntities.push_back(owner);
		owner->x = bearer->x; owner->y = bearer->y;
		if(bearer == engine.player) {
			engine.gui->message(TCODColor::lightGrey, "You have dropped the %s", owner->name);
		}
		else {
			engine.gui->message(TCODColor::lightGrey, "The %s has dropped a %s", bearer->name, owner->name);
		}
	}
}


// HEALER

Healer::Healer(float amt) : amt(amt) {}

bool Healer::use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer) {
	if(bearer->mortal) {
		float amtHealed(bearer->mortal->heal(amt));
		if(amtHealed > 0) { return Loot::use(owner, bearer); }
	}
	return false;
}


// DAMAGE SPELL

DamageSpell::DamageSpell(float range, float dmg) : range(range), dmg(dmg) {}


// LIGHTNING BOLT

LightningBolt::LightningBolt(float range, float dmg) : DamageSpell(range, dmg) {}

bool LightningBolt::use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer) {
	std::shared_ptr<Entity> closestMonster = engine.getClosestMonster(bearer->x, bearer->y, range);
	if(!closestMonster) { engine.gui->message(TCODColor::lightGrey, "No enemy is close enough to strike."); return false; }
	engine.gui->message(TCODColor::azure, "An azure bolt splits the air with a crack.");
	engine.gui->message(TCODColor::azure, "The %s is struck for %g damage!", closestMonster->name, dmg);
	closestMonster->mortal->takeDamage(closestMonster, dmg);
	return Loot::use(owner, bearer);
}


// FIREBALL

Fireball::Fireball(float range, float dmg, float radius) : DamageSpell(range, dmg), radius(radius) {}

bool Fireball::use(std::shared_ptr<Entity> owner, std::shared_ptr<Entity> bearer) {
	engine.gui->message(TCODColor::cyan, "Left-click to cast fireball,\nor right-click to cancel.");
	int x, y;
	if(!engine.pickTile(&x, &y, range, radius)) { return false; }
	engine.gui->message(TCODColor::flame, "The fireball explodes burning everything in radius %g!", radius);
	for(auto & ent : engine.activeEntities) {
		if(ent->getDistance(x, y) <= radius) {
			engine.gui->message(TCODColor::flame, "The %s is burned by the fireball for %g damage!", ent->name, dmg);
			ent->mortal->takeDamage(ent, dmg);
		}
	}
	return Loot::use(owner, bearer);
}
