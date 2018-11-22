#include "main.hpp"

Combat::Combat(float atk) : atk(atk) {}

void Combat::attack(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target) {
	if(target->mortal && !target->mortal->isDead()) {
		float dmg = atk - target->mortal->def;
		if(dmg > 0.0) {
			// std::cout << "The " << attacker->name << " attacks the " << target->name << " for " << dmg << " damage." << std::endl;
			// printf("the %s hits the %s for %g damage.\n", attacker->name, target->name, dmg);
			engine.gui->message(attacker == engine.player ? TCODColor::red : TCODColor::lightGrey, "The %s attacks the %s for %g damage!", attacker->name, target->name, dmg);
			target->mortal->takeDamage(target, atk);
		}
		else {
			// std::cout << "The " << attacker->name << " is too weak to harm the " << target->name << "." << std::endl;
			// printf("The %s is too weak to harm the %s\n", attacker->name, target->name);
			engine.gui->message(TCODColor::lightGrey, "The %s has no effect on the %s !", attacker->name, target->name);
		}
	}
	else {
		// std::cout << "The " << target->name << " is impervious to damage." << std::endl;
		// printf("The %s is impervious to damage.\n", target->name);
		engine.gui->message(TCODColor::lightGrey, "The %s is too weak to hurt the %s !", attacker->name, target->name);
	}
}
