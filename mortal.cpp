#include "main.hpp"

Mortal::Mortal() {}

Mortal::Mortal(int maxHp, const char * corpseName) : maxHp(maxHp), hp(maxHp), corpseName(corpseName) {}

Mortal::Mortal(const Mortal & obj) {
	maxHp = obj.maxHp;
	hp = obj.hp;
	corpseName = _strdup(obj.corpseName);
}

int Mortal::takeDamage(shared_ptr<Entity> owner, int dmg) {
	if(dmg > 0) {
		hp -= dmg;
		if(hp <= 0) { engine.notifyDeath(owner); }
	}
	else { dmg = 0; }
	return dmg;
}

// See how much a heal would affect the Mortal without actually applying the healing.
int Mortal::predictHeal(int amount) {
	int _hp = hp;
	_hp += amount;
	if(_hp > maxHp) { amount -= _hp - maxHp; }
	return amount;
}

int Mortal::heal(int amount) {
	hp += amount;
	if(hp > maxHp) { amount -= hp - maxHp; hp = maxHp; }
	return amount;
}

void Mortal::die(shared_ptr<Entity> owner) {
	owner->ch = '%';
	owner->color = TCODColor::darkRed;
	owner->name = corpseName;
	owner->blocks = false;	
}


pcMortal::pcMortal() {}

pcMortal::pcMortal(int maxHp, const char * corpseName) : Mortal(maxHp, corpseName) {}

shared_ptr<Mortal> pcMortal::clone() const {
	return std::make_shared<pcMortal>(*this);
}

void pcMortal::die(shared_ptr<Entity> owner) {
	engine.ui->message(TCODColor::red, "You've been slain nerd.");
	Mortal::die(owner);
	engine.gameState = engine.LOSE;
}


npcMortal::npcMortal() {}

npcMortal::npcMortal(int maxHp, const char * corpseName) : Mortal(maxHp, corpseName) {}

shared_ptr<Mortal> npcMortal::clone() const {
	return std::make_shared<npcMortal>(*this);
}

void npcMortal::die(shared_ptr<Entity> owner) {
	engine.ui->message(TCODColor::lightGrey, "The %s has perished like a nerd.", owner->name);
	Mortal::die(owner);
}
