#include "main.hpp"

// Probably want to change a lot of the "use" methods. Seems wrong to send messages to ui from here.

bool Loot::canCollect(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	if(bearer->container && bearer->container->canAdd(owner)) return true;
	return false;
}

bool Loot::collect(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	if(bearer->container && bearer->container->add(owner)) {
		auto it = std::find(engine.entityList.begin(), engine.entityList.end(), owner);
		engine.entityList.erase(it);
		it = std::find(engine.inactiveEntities.begin(), engine.inactiveEntities.end(), owner);
		engine.inactiveEntities.erase(it);
		return true;
	}
	return false;
}

void Loot::use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	if(bearer->container) { bearer->container->del(owner); }
}

void Loot::drop(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	if(bearer->container) {
		bearer->container->del(owner);
		engine.entityList.push_back(owner); engine.inactiveEntities.push_back(owner);
		owner->x = bearer->x; owner->y = bearer->y;
		if(bearer == engine.player) {
			engine.ui->message(TCODColor::lightGrey, "You have dropped the %s", owner->name);
		}
		else {
			engine.ui->message(TCODColor::lightGrey, "The %s has dropped a %s", bearer->name, owner->name);
		}
	}
}


// HEALER

Healer::Healer(int amt) : amt(amt) {}

Healer::Healer(const Healer & obj) { amt = obj.amt; }

shared_ptr<Loot> Healer::clone() const {
	return std::make_shared<Healer>(*this);
}

bool Healer::canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	if(bearer->mortal) {
		int amtHealed = bearer->mortal->predictHeal(amt);
		if(amtHealed > 0) return true;
	}
	return false;
}

void Healer::use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	if(bearer->mortal) {
		int amtHealed = bearer->mortal->heal(amt);
		if(amtHealed > 0) { 
			engine.ui->message(TCODColor::lightGreen, "%s heals for %d.", bearer->name, amtHealed);
			Loot::use(owner, bearer);
		}
	}
}


// DAMAGE SPELL

DamageSpellItem::DamageSpellItem(float range, int dmg, float radius) : range(range), dmg(dmg), radius(radius) {}

DamageSpellItem::DamageSpellItem(const DamageSpellItem & obj) {
	range = obj.range;
	dmg = obj.dmg;
	radius = obj.radius;
}


// LIGHTNING BOLT

LightningBolt::LightningBolt(float range, int dmg) : DamageSpellItem(range, dmg) {}

shared_ptr<Loot> LightningBolt::clone() const {
	return std::make_shared<LightningBolt>(*this);
}

bool LightningBolt::canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	shared_ptr<Entity> closestMonster = engine.getClosestMonster(bearer->x, bearer->y, range);
	if(!closestMonster) { engine.ui->message(TCODColor::lightGrey, "No enemy is close enough to strike."); return false; }
	return true;
}

void LightningBolt::use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	shared_ptr<Entity> closestMonster = engine.getClosestMonster(bearer->x, bearer->y, range);
	if(!closestMonster) return;
	engine.ui->message(TCODColor::azure, "An azure bolt splits the air with a crack.");
	engine.ui->message(TCODColor::azure, "The %s is struck for %d damage!", closestMonster->name, dmg);
	closestMonster->mortal->takeDamage(closestMonster, dmg);
	Loot::use(owner, bearer);
}


// FIREBALL

Fireball::Fireball(float range, int dmg, float radius) : DamageSpellItem(range, dmg, radius) {}

Fireball::Fireball(const Fireball & obj) : DamageSpellItem(obj.range, obj.dmg, obj.radius) {
	x = obj.x;
	y = obj.y;
}

shared_ptr<Loot> Fireball::clone() const {
	return std::make_shared<Fireball>(*this);
}

bool Fireball::canUse(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	engine.ui->message(TCODColor::cyan, "Left-click to cast fireball,\nor right-click to cancel.");
	if(!engine.pickTile(&x, &y, range, radius)) { return false; }
	return true;
}

void Fireball::use(shared_ptr<Entity> owner, shared_ptr<Entity> bearer) {
	engine.ui->message(TCODColor::flame, "The fireball explodes burning everything in radius %g!", radius);
	for(auto & ent : engine.activeEntities) {
		if(ent->getDistance(x, y) <= radius) {
			engine.ui->message(TCODColor::flame, "The %s is burned by the fireball for %d damage!", ent->name, dmg);
			ent->mortal->takeDamage(ent, dmg);
		}
	}
	Loot::use(owner, bearer);
}
