#include "main.hpp"

SpellCaster::SpellCaster() 
	: maxCharges1(5),
	maxCharges2(3),
	maxCharges3(2) {
	charges = new int[3] {maxCharges1, maxCharges2, maxCharges3};
}

SpellCaster::SpellCaster(const SpellCaster & obj) {
	maxCharges1 = obj.maxCharges1;
	maxCharges2 = obj.maxCharges2;
	maxCharges3 = obj.maxCharges3;
	charges = new int[sizeof(obj.charges)];
	for(int i = 0; i < sizeof(charges); ++i) {
		charges[i] = obj.charges[i];
	}
	for(auto & spell : obj.spellList) {
		spellList.push_back(spell->clone());
	}
}

SpellCaster::~SpellCaster() { spellList.clear(); delete [] charges; }

int SpellCaster::getCharges(int level) const {
	if(level > sizeof(charges) + 1) return 0;
	return charges[level - 1];
}

int SpellCaster::getMaxCharges(int level) const {
	switch(level) {
	case 1: return maxCharges1; break;
	case 2: return maxCharges2; break;
	case 3: return maxCharges3; break;
	}
	return 0;
}

void SpellCaster::useCharge(int level) { if(charges[level - 1] > 0) charges[level - 1] -= 1; }
