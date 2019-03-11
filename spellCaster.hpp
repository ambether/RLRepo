#pragma once

class SpellCaster {
public:
	vector<shared_ptr<Spell>> spellList;

	SpellCaster();
	SpellCaster(const SpellCaster & obj); // Copy ctor
	~SpellCaster();

	int getCharges(int level) const;
	int getMaxCharges(int level) const;
	void useCharge(int level);
private:
	int maxCharges1,
		maxCharges2,
		maxCharges3;
	int * charges;
};