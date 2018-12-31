#include "main.hpp"

Combat::Combat(float atk) : atk(atk) {}

void Combat::attack(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target) {
	if(target->mortal && !target->mortal->isDead()) {
		float dmg = atk - target->mortal->def;
		if(dmg > 0.0) {
			engine.ui->message(attacker == engine.player ? TCODColor::red : TCODColor::lightGrey, "The %s attacks the %s for %g damage!", attacker->name, target->name, dmg);
			target->mortal->takeDamage(target, atk);
		}
		else { engine.ui->message(TCODColor::lightGrey, "The %s has no effect on the %s !", attacker->name, target->name); }
	}
	else { engine.ui->message(TCODColor::lightGrey, "The %s is too weak to hurt the %s !", attacker->name, target->name); }
}
