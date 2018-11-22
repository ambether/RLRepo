#pragma once

class Combat {
public:
	float atk;

	Combat(float atk);

	void attack(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target);
};