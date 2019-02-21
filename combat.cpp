#include "main.hpp"

//Combat::Combat(float atk) : atk(atk) {}

Combat::Combat(int dexterity, int skill, int strength) : dexterity(dexterity), skill(skill), strength(strength) {}

int Combat::getDex() const { return dexterity; }

int Combat::getSkl() const { return skill; }

int Combat::getStr() const { return strength; }

void Combat::attack(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target) {
	// Attackers start with a 40% chance to hit. The chance goes up by 5% for each point of SKL they have.
	std::random_device seed;
	std::shared_ptr<TCODRandom> rng = std::make_shared<TCODRandom>(seed()); // Initialize a prng with a (psuedo)random seed.
	int toHitRoll = rng->getInt(1, 100);
	if(toHitRoll + (skill * SKL_MOD) >= (100 - BASE_HIT_CHANCE)) {
		// The attacker has hit so roll for the target's dodge
		// Targets start with a 15% chance to dodge. The chance goes up by 5% for each point of DEX they have.
		int toDodgeRoll = rng->getInt(1, 100);
		if(toDodgeRoll + (dexterity * DEX_MOD) >= (100 - BASE_DODGE_CHANCE)) {
			// The attacker misses. No damage is done.
			engine.ui->message(attacker == engine.player ? TCODColor::red : TCODColor::lightGrey, "%s strikes out but the %s dodges away!", attacker->name, target->name);
		}
		else { 
			// No dodge happened
			engine.ui->message(attacker == engine.player ? TCODColor::green : TCODColor::red, "The %s attacks the %s for %d damage.", attacker->name, target->name, strength);
			target->mortal->takeDamage(target, strength);
		}

	}
	else { 
		// The attacker missed the attack
		engine.ui->message(attacker == engine.player ? TCODColor::red : TCODColor::lightGrey, "%s swings but misses!", attacker->name);
	}
}