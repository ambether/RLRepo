#pragma once

class Combat {
public:
	Combat(int dexterity, int skill, int strength);

	int getDex() const;
	int getSkl() const;
	int getStr() const;

	void attack(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target);
private:
	int dexterity,	// DEX
		skill,		// SKL
		strength;	// STR
	static const int BASE_HIT_CHANCE = 40,		// The base percent chance to hit
					 BASE_DODGE_CHANCE = 15;	// The base percent chance to dodge

	static const int SKL_MOD = 5, // The percent added to hit per point of skill
					 DEX_MOD = 5; // The percent added to dodge per point of dexterity
};