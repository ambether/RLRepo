#include "main.hpp"

// Spell base class

Spell::Spell(std::string name, int level, float range, float radius, const TCODColor color)
	: name(name),
	level(level),
	range(range),
	radius(radius),
	color(color) {}

Spell::Spell(std::string name, int level, float range, const TCODColor color) 
	: name(name),
	level(level),
	range(range),
	radius(0), 
	color(color) {}

int Spell::getLevel() const { return level; }

float Spell::getRange() const { return range; }

float Spell::getRadius() const { return radius; }

std::string Spell::getName() const { return name; }


// Damage spell

DamageSpell::DamageSpell(std::string name, int level, float range, float radius, int damage, const TCODColor color)
	: Spell(name, level, range, radius, color),
	damage(damage) {}

DamageSpell::DamageSpell(std::string name, int level, float range, int damage, const TCODColor color)
	: Spell(name, level, range, color), 
	damage(damage) {}

bool DamageSpell::canUse(std::shared_ptr<Entity> owner) {
	if(owner->spellCaster->getCharges(level) > 0) {
		engine.ui->message(TCODColor::cyan, "Left-click on an enemy to cast %s\nor right-click to cancel.", name.c_str());
		if(!engine.pickTile(&tx, &ty, range, radius)) return false;
		if(radius == 0 && engine.getMonster(tx, ty) == nullptr) return false;
		return true;
	}
	engine.ui->message(TCODColor::red, "Insufficient spell points.");
	return false;
}

void DamageSpell::use(std::shared_ptr<Entity> owner) {
	owner->spellCaster->useCharge(level);
	for(auto & ent : engine.activeEntities) {
		if(ent->getDistance(tx, ty) <= radius) {
			engine.ui->message(color, "%s takes %d damage!", ent->name, damage);
			ent->mortal->takeDamage(ent, damage);
		}
	}
}

int DamageSpell::getDamage() const { return damage; }


