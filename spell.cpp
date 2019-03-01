#include "main.hpp"

Spell::Spell() : 
	name(nullptr), 
	level(0),
	range(0.0f), 
	radius(0.0f), 
	color(TCODColor::white),
	targetType(SELF), 
	_damageDealer(nullptr),
	_healer(nullptr) {}

Spell::Spell(const Spell & obj) {
	name = _strdup(obj.name);
	level = obj.level;
	range = obj.range;
	radius = obj.radius;
	color = obj.color;
	targetType = obj.targetType;
	if(obj._damageDealer) {
		_damageDealer = new _DamageDealer();
		_damageDealer->damage = obj._damageDealer->damage; 
	}
	else _damageDealer = nullptr;
	if(obj._healer) {
		_healer = new _Healer();
		_healer->amount = obj._healer->amount;
	}
	else _healer = nullptr;
}

bool Spell::canUse(shared_ptr<Entity> owner) {
	if(owner->spellCaster->getCharges(level) > 0) {
		switch(targetType) {
		case SELF: 
		{
			if(_healer) {
				if(owner->mortal->predictHeal(_healer->amount) == 0){
					engine.ui->message(TCODColor::white, "Health is already full.");
					return false;
				}
				return true;
			}
			return false;
		}
		case CLOSEST: 
		{
			shared_ptr<Entity> closestMonster = engine.getClosestMonster(owner->x, owner->y, range);
			if(!closestMonster) { engine.ui->message(TCODColor::lightGrey, "No enemy is close enough."); return false; }
			return true;
		}
		case RANGED:
		{
			engine.ui->message(TCODColor::white, "Left click target to cast %s\nor right-click to cancel.", name);
			if(!engine.pickTile(&tx, &ty, range, radius)) return false;
			if(radius == 0 && engine.getMonster(tx, ty) == nullptr) return false;
			return true;
		} 
		} // end switch
	}
	engine.ui->message(TCODColor::red, "Insufficient spell points.");
	return false;
}

void Spell::use(shared_ptr<Entity> owner) {
	owner->spellCaster->useCharge(level);
	if(targetType == SELF) {
		if(_healer) { 
			engine.ui->message(TCODColor::white, "%s heals for %d.", owner->name, owner->mortal->predictHeal(_healer->amount));
			owner->mortal->heal(_healer->amount);
		}
	}
	else if(targetType == CLOSEST) {
		shared_ptr<Entity> closestMonster = engine.getClosestMonster(owner->x, owner->y, range);
		if(closestMonster && _damageDealer) {
			engine.ui->message(color, "The %s takes %d damage!", closestMonster->name, _damageDealer->damage);
			closestMonster->mortal->takeDamage(closestMonster, _damageDealer->damage);
		}
	}
	else if(targetType == RANGED) {
		if(_damageDealer) {
			for(auto & ent : engine.activeEntities) {
				if(ent->getDistance(tx, ty) <= radius) {
					engine.ui->message(color, "The %s takes %d damage!", ent->name, _damageDealer->damage);
					ent->mortal->takeDamage(ent, _damageDealer->damage);
				}
			}
		}
	}
}

void Spell::initDamageDealer() { _damageDealer = new _DamageDealer(); }

void Spell::initHealer() { _healer = new _Healer(); }

void Spell::addDamage(int damage) {
	if(_damageDealer) _damageDealer->damage = damage;
}

void Spell::addHeal(int heal) {
	if(_healer) _healer->amount = heal;
}