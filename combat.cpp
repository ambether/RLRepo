#include "main.hpp"

Combat::Combat() {}

Combat::Combat(const Combat & obj) { damageDice = obj.damageDice; }

void Combat::setDamageDice(TCOD_dice_t damageDice) { this->damageDice = damageDice; }

void Combat::attack(shared_ptr<Entity> attacker, shared_ptr<Entity> target) {
	std::random_device seed;
	shared_ptr<TCODRandom> rng = std::make_shared<TCODRandom>(seed());
	// Add to-hit stuff here
	// Add dodge stuff here
	int damage = rng->diceRoll(damageDice);
	engine.ui->message(attacker == engine.player ? TCODColor::green : TCODColor::red, "The %s attacks the %s for %d damage.", attacker->name, target->name, damage);
	target->mortal->takeDamage(target, damage);
}

