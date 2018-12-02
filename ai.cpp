#include "main.hpp"

Ai::Ai(int speed) : speed(speed), energy(0) {}

void Ai::gainEnergy() { 
	energy += speed; 
}

void Ai::spendEnergy() { if(energy >= 100) energy -= 100; }


playerAi::playerAi() : Ai(100) {}

void playerAi::update(std::shared_ptr<Entity> owner) {
	if(owner->mortal && !owner->mortal->isDead()) {
		if(energy >= 100) {
			int dx = 0, dy = 0;
			switch(engine.lastKey.vk) {
			case TCODK_UP:		dy = -1; break;
			case TCODK_DOWN:	dy =  1; break;
			case TCODK_LEFT:	dx = -1; break;
			case TCODK_RIGHT:	dx =  1; break;
			case TCODK_CHAR: handleActionKey(owner, engine.lastKey.c); break;
			default: break;
			}
			if(dx != 0 || dy != 0) {
				if(moveOrAttack(owner, dx, dy)) { engine.dungeon->computeFov(); } // only returns true if it moves the Entity
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
					//if(ent->loot->collect(ent, owner)) {
					if(ent->loot->canCollect(ent, owner)) {
						found = true;
						//engine.gui->message(TCODColor::lightGrey, "You collect the %s.", lootName);
						spendEnergy();
						engine.addAction(std::make_shared<CollectAction>(ent, owner));
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

bool playerAi::moveOrAttack(std::shared_ptr<Entity> owner, int dx, int dy) {
	if(engine.dungeon->isWall(owner->x + dx, owner->y + dy)) { return false; }
	spendEnergy();
	engine.addAction(std::make_shared<MoveAction>(owner, dx, dy));
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
	if(owner->mortal && owner->mortal->isDead()) { return; }

	while(energy >= 100) {
		if(engine.dungeon->isInFov(owner->x, owner->y)) { moveCount = TRACK_TURNS; }
		else { --moveCount; }

		if(moveCount > 0) {	moveOrAttack(owner); }
		else { idle(owner); }
	}
}

void mobAi::moveOrAttack(std::shared_ptr<Entity> owner) {
	spendEnergy();
	engine.addAction(std::make_shared<MoveAtPlayerAction>(owner));
}

void mobAi::idle(std::shared_ptr<Entity> owner) {
	spendEnergy();
	engine.addAction(std::make_shared<IdleAction>(owner));
}