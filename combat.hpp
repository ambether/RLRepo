#pragma once

class Combat {
public:
	Combat();
	Combat(const Combat & obj); // Copy ctor

	void setDamageDice(TCOD_dice_t damageDice);
	void attack(shared_ptr<Entity> attacker, shared_ptr<Entity> target);
private:
	TCOD_dice_t damageDice;
	
	enum Stance {NONE, LOW, MID, HIGH } stance;
	static enum StanceComparison { DRAW, A_WIN, B_WIN };
	static StanceComparison compareStance(Stance stanceA, Stance stanceB);
	const static StanceComparison stanceTable [4][4];
};