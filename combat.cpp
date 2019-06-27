#include "main.hpp"
														  
const Combat::StanceComparison Combat::stanceTable[4][4] = { 
		   /*NONE   LOW    MID    HIGH*/
/*NONE*/	{DRAW,  B_WIN, B_WIN, B_WIN}, 
/*LOW*/		{A_WIN, DRAW,  B_WIN, A_WIN},
/*MID*/		{A_WIN, A_WIN, DRAW,  B_WIN},
/*HIGH*/	{A_WIN, B_WIN, A_WIN, DRAW} };

Combat::Combat() {}

Combat::Combat(const Combat & obj) { damageDice = obj.damageDice; }

void Combat::setDamageDice(TCOD_dice_t damageDice) { this->damageDice = damageDice; }

void Combat::attack(shared_ptr<Entity> attacker, shared_ptr<Entity> target) {
	std::random_device seed;
	shared_ptr<TCODRandom> rng = std::make_shared<TCODRandom>(seed());
	// Compare stances
	// If attacker's stance beats target's, deal damage
	int damage = rng->diceRoll(damageDice);
	engine.ui->message(attacker == engine.player ? TCODColor::green : TCODColor::red, "The %s attacks the %s for %d damage.", attacker->name, target->name, damage);
	target->mortal->takeDamage(target, damage);
}

Combat::StanceComparison Combat::compareStance(Stance stanceA, Stance stanceB) {
	return stanceTable[stanceA][stanceB];
}
