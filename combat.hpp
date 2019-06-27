#pragma once

class Combat {
public:
	Combat();
	Combat(const Combat & obj); // Copy ctor

	void setDamageDice(TCOD_dice_t damageDice);
	void attack(shared_ptr<Entity> attacker, shared_ptr<Entity> target);
private:
	TCOD_dice_t damageDice;
};