#include "main.hpp"

Ai::Ai(int speed) : speed(speed), energy(0) {}

void Ai::gainEnergy() { energy += speed; }

void Ai::spendEnergy() { if(energy >= 100) energy -= 100; }


playerAi::playerAi() : Ai(100) {}

void playerAi::update(std::shared_ptr<Entity> owner) {
	if(owner->mortal && !owner->mortal->isDead()) {
		if(energy >= 100) {
			int dx = 0, dy = 0;
			switch(engine.lastKey.vk) {
			case TCODK_UP:		dy = -1; break;
			case TCODK_DOWN:	dy = 1; break;
			case TCODK_LEFT:	dx = -1; break;
			case TCODK_RIGHT:	dx = 1; break;
			case TCODK_CHAR: handleActionKey(owner, engine.lastKey.c); break;
			default: break;
			}
			if(dx != 0 || dy != 0) {
				if(moveOrAttack(owner, owner->x + dx, owner->y + dy)) { // only returns true if it moves the actor
					engine.dungeon->computeFov();
				}
			}
		}
		if(energy < 100) engine.gameState = Engine::TURN;
	}
}

void playerAi::handleActionKey(std::shared_ptr<Entity> owner, int ascii) { // TODO: make the use of/interaction with items part of the Action system.
	switch(ascii) {
	case 'c': // Collect item
		{
			bool found = false;
			for(auto & ent : engine.inactiveEntities) {
				if(ent->loot && ent->x == owner->x && ent->y == owner->y) {
					auto lootName = ent->name;
					if(ent->loot->collect(ent, owner)) {
						found = true;
						engine.gui->message(TCODColor::lightGrey, "You collect the %s.", lootName);
						spendEnergy();
						break;
					}
					else if(!found) {
						found = true;
						engine.gui->message(TCODColor::red, "Your inventory is full.");
					}
				}
			}
			if(!found) { engine.gui->message(TCODColor::lightGrey, "There's nothing here."); }
		}
		break;
	case 'i': // Access inventory
		{
			std::shared_ptr<Entity> item = chooseFromInv(owner);
			if(item) {
				if(item->loot->use(item, owner)) { spendEnergy(); }
			}
		}
		break;
	}
}

bool playerAi::moveOrAttack(std::shared_ptr<Entity> owner, int tx, int ty) {
	if(engine.dungeon->isWall(tx, ty)) { return false; }
	spendEnergy();
	for(auto & ent : engine.entityList) {
		if(ent->mortal) {
			if(!ent->mortal->isDead() && ent->x == tx && ent->y == ty) {
				//owner->combat->attack(owner, ent);
				engine.actionQueue.addAction(std::make_shared<AttackAction>(owner, ent));
				return false;
			}
			else if(ent->x == tx && ent->y == ty) { engine.gui->message(TCODColor::white, "There is a %s here.", ent->name); }
		}
		else if(ent->x == tx && ent->y == ty) {	engine.gui->message(TCODColor::white, "There is a %s here.", ent->name); }
	}
	//owner->x = tx;
	//owner->y = ty;
	engine.actionQueue.addAction(std::make_shared<MoveAction>(owner, tx, ty));
	return true;
}

std::shared_ptr<Entity> playerAi::chooseFromInv(std::shared_ptr<Entity> owner) {
	static const int invW = 50, invH = 28;
	static TCODConsole con(invW, invH);

	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, invW, invH, true, TCOD_BKGND_DEFAULT, "Inventory");
	con.setDefaultForeground(TCODColor::white);

	int is = 'a';
	int i = 1;

	for(auto & item : owner->container->inventory) {
		con.print(2, i, "(%c) %s", is, item->name);
		++i; ++is;
	}
	
	TCODConsole::blit(&con, 0, 0, invW, invH, TCODConsole::root, engine.screenWidth / 2 - invW / 2, engine.screenHeight / 2 - invH / 2);
	TCODConsole::flush();

	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if(key.vk == TCODK_CHAR) {
		int itemIndex = key.c - 'a';
		if(itemIndex >= 0 && itemIndex < owner->container->inventory.size()) {
			return owner->container->inventory.at(itemIndex);
		}
	}
	return NULL;
}


static const int TRACK_TURNS(3);

mobAi::mobAi(int speed) : Ai(speed) {}

void mobAi::update(std::shared_ptr<Entity> owner) {
	if(owner->mortal) {
		if(owner->mortal->isDead()) { return; }
	}
	while(energy >= 100) {
		if(engine.dungeon->isInFov(owner->x, owner->y)) { moveCount = TRACK_TURNS; }
		else { --moveCount; }
		if(moveCount > 0) { moveOrAttack(owner, engine.player->x, engine.player->y); }
		spendEnergy();
	}
}

void mobAi::moveOrAttack(std::shared_ptr<Entity> owner, int tx, int ty) {
	int dx = tx - owner->x,
		dy = ty - owner->y,
		sdx = (dx > 0 ? 1 : -1), 
		sdy = (dy > 0 ? 1 : -1), 
		distance((int)sqrt(dx*dx + dy*dy));
	if(distance >= 2) {
		dx = (int)(round(dx / distance));
		dy = (int)(round(dy / distance));
		//if(engine.dungeon->canWalk(owner->x + dx, owner->y + dy)) { owner->x += dx; owner->y += dy; }
		//else if(engine.dungeon->canWalk(owner->x + sdx, owner->y)) { owner->x += sdx; }
		//else if(engine.dungeon->canWalk(owner->x, owner->y + sdy)) { owner->y += sdy; }
		if(engine.dungeon->canWalk(owner->x + dx, owner->y + dy)) { engine.actionQueue.addAction(std::make_shared<MoveAction>(owner, owner->x+dx, owner->y+ dy)); }
		else if(engine.dungeon->canWalk(owner->x + sdx, owner->y)) { engine.actionQueue.addAction(std::make_shared<MoveAction>(owner, owner->x + sdx, owner->y)); }
		else if(engine.dungeon->canWalk(owner->x, owner->y + sdy)) { engine.actionQueue.addAction(std::make_shared<MoveAction>(owner, owner->x, owner->y + sdy)); }
	}
	//else if(owner->combat) { owner->combat->attack(owner, engine.player); }
	else if(owner->combat) { engine.actionQueue.addAction(std::make_shared<AttackAction>(owner, engine.player)); }
}