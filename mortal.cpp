#include "main.hpp"

Mortal::Mortal(float maxHp, float def, const char * corpseName) : maxHp(maxHp), hp(maxHp), def(def), corpseName(corpseName) {}

float Mortal::takeDamage(std::shared_ptr<Entity> owner, float dmg) {
	dmg -= def;
	if(dmg > 0) {
		hp -= dmg;
		if(hp <= 0) { engine.notifyDeath(owner); }
	}
	else { dmg = 0; }
	return dmg;
}

void Mortal::die(std::shared_ptr<Entity> owner) {
	owner->ch = '%';
	owner->color = TCODColor::darkRed;
	owner->name = corpseName;
	owner->blocks = false;	
}

// See how much a heal would affect the Mortal without actually applying the healing.
float Mortal::predictHeal(float amount) {
	int _hp = hp;
	_hp += amount;
	if(_hp > maxHp) { amount -= _hp - maxHp; }
	return amount;
}

float Mortal::heal(float amount) {
	hp += amount;
	if(hp > maxHp) { amount -= hp - maxHp; hp = maxHp; }
	return amount;
}


pcMortal::pcMortal(float maxHp, float def, const char * corpseName) : Mortal(maxHp, def, corpseName) {}

void pcMortal::die(std::shared_ptr<Entity> owner) {
	engine.gui->message(TCODColor::red, "You've been slain nerd.");
	Mortal::die(owner);
	engine.gameState = engine.LOSE;
}


npcMortal::npcMortal(float maxHp, float def, const char * corpseName) : Mortal(maxHp, def, corpseName) {}

void npcMortal::die(std::shared_ptr<Entity> owner) {
	engine.gui->message(TCODColor::lightGrey, "The %s has perished like a nerd.", owner->name);
	Mortal::die(owner);
}
