#include "main.hpp"

Mortal::Mortal(float maxHp, float def, const char * corpseName) : maxHp(maxHp), hp(maxHp), def(def), corpseName(corpseName) {}

float Mortal::takeDamage(std::shared_ptr<Entity> owner, float dmg) {
	dmg -= def;
	if(dmg > 0) {
		hp -= dmg;
		if(hp <= 0) { die(owner); }
	}
	else { dmg = 0; }
	return dmg;
}

void Mortal::die(std::shared_ptr<Entity> owner) {
	owner->ch = '%';
	owner->color = TCODColor::darkRed;
	owner->name = corpseName;
	owner->blocks = false;
	
	engine.notifyDeath(owner); // Tells the engine that this entity has died and should be rendered under living entities
}

float Mortal::heal(float amount) {
	hp += amount;
	if(hp > maxHp) { amount -= hp - maxHp; hp = maxHp; }
	return amount;
}

pcMortal::pcMortal(float maxHp, float def, const char * corpseName) : Mortal(maxHp, def, corpseName) {}

void pcMortal::die(std::shared_ptr<Entity> owner) {
	// printf("You have been slain, nerd.\n");
	// std::cout << "You have been slain nerd." << std::endl;
	engine.gui->message(TCODColor::red, "You've been slain nerd.");
	Mortal::die(owner);
	engine.gameState = engine.LOSE;
}

npcMortal::npcMortal(float maxHp, float def, const char * corpseName) : Mortal(maxHp, def, corpseName) {}

void npcMortal::die(std::shared_ptr<Entity> owner) {
	// printf("The %s has perished like a nerd.\n", owner->name);
	//std::cout << "The " << owner->name << " has perished like a nerd." << std::endl;
	engine.gui->message(TCODColor::lightGrey, "The %s has perished like a nerd.", owner->name);
	Mortal::die(owner);
}
