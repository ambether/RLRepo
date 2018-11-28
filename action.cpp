#include "main.hpp"

MoveAction::MoveAction(std::shared_ptr<Entity> owner, int tx, int ty) : owner(owner), tx(tx), ty(ty) {}

void MoveAction::execute() { owner->x = tx; owner->y = ty; }


AttackAction::AttackAction(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> target) : attacker(attacker), target(target) {}

void AttackAction::execute() { attacker->combat->attack(attacker, target); }
