#include "main.hpp"

Ai::Ai() : speed(0), energy(0) {}

Ai::Ai(int speed) : speed(speed), energy(0) {}

void Ai::gainEnergy() { energy += speed; }

void Ai::setSpeed(int speed) { this->speed = speed; }

int Ai::getSpeed() const { return speed; }

void Ai::spendEnergy() { if(energy >= 100) energy -= 100; }


playerAi::playerAi() : Ai(100) {}

void playerAi::update(std::shared_ptr<Entity> owner) {
	if(owner->mortal && !owner->mortal->isDead()) {
		if(energy >= 100) {
			int dx = 0, dy = 0;
			switch(engine.lastKey.vk) {
			case TCODK_UP:		dy = -1; break;
			case TCODK_KP8:		dy = -1; break;
			case TCODK_DOWN:	dy =  1; break;
			case TCODK_KP2:		dy =  1; break;
			case TCODK_LEFT:	dx = -1; break;
			case TCODK_KP4:		dx = -1; break;
			case TCODK_RIGHT:	dx =  1; break;
			case TCODK_KP6:		dx =  1; break;
			case TCODK_KP5:		handleActionKey(owner, '.'); break;
			case TCODK_CHAR:	handleActionKey(owner, engine.lastKey.c); break;
			default: break;
			}
			if(dx != 0 || dy != 0) {
				if(moveOrAttack(owner, dx, dy)) { engine.dungeon->computeFov(); } // only returns true if it moves the Entity
			}
		}
		if(energy < 100) engine.gameState = Engine::TURN;
	}
}

void playerAi::handleActionKey(std::shared_ptr<Entity> owner, int ascii) {
	switch(ascii) {
	case 'g': // Grab item
		{
			bool found = false;
			for(auto & ent : engine.inactiveEntities) {
				if(ent->loot && ent->x == owner->x && ent->y == owner->y) {
					auto lootName = ent->name;
					if(ent->loot->canCollect(ent, owner)) {
						found = true;
						spendEnergy();
						engine.addAction(std::make_shared<CollectAction>(ent, owner));
						break;
					}
					else if(!found) {
						found = true;
						engine.ui->message(TCODColor::red, "Your inventory is full.");
					}
				}
			}
			if(!found) { engine.ui->message(TCODColor::lightGrey, "There's nothing here."); }
		}
		break;
	case 'i': // Access inventory
		{
			std::shared_ptr<Entity> item = chooseFromInv(owner);
			if(item != nullptr && item->loot->canUse(item, owner)) { 
				spendEnergy();
				engine.addAction(std::make_shared<UseAction>(item, owner));
			}
		}
		break;
	case '.': // Idle
		{
			spendEnergy();
			engine.addAction(std::make_shared<IdleAction>(owner));
		}
		break;
	case 'c': // Interact  // TODO: figure out a more logical key
		{
			TCOD_key_t key = chooseInteractDirection();
			int dx = 0, dy = 0;
			switch(key.vk) {
				case TCODK_UP:		dy = -1; break;
				case TCODK_KP8:		dy = -1; break;
				case TCODK_DOWN:	dy =  1; break;
				case TCODK_KP2:		dy =  1; break;
				case TCODK_LEFT:	dx = -1; break;
				case TCODK_KP4:		dx = -1; break;
				case TCODK_RIGHT:	dx =  1; break;
				case TCODK_KP6:		dx =  1; break;
				case TCODK_ESCAPE:
				default:			engine.ui->message(TCODColor::grey, "Canceled."); return;
			}
			bool found = false;
			for(auto & ent : engine.entityList) {
				if(!ent->interaction) continue;
				else if(ent->x == owner->x + dx && ent->y == owner->y + dy) {
					found = true;
					spendEnergy();
					engine.addAction(std::make_shared<InteractAction>(ent, owner));
					break;
				}
			}
			if(!found) engine.ui->message(TCODColor::grey, "There is nothing to interact with there.");
		} 
		break;
	case 'f': // Fire spell
		{
			std::shared_ptr<Spell> spell = chooseFromSpells(owner);
			if(spell != nullptr && spell->canUse(owner)) {
				spendEnergy();
				engine.addAction(std::make_shared<CastAction>(spell, owner));
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
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr, true);
	if(key.vk == TCODK_CHAR) {
		int itemIndex = key.c - 'a';
		if(itemIndex >= 0 && itemIndex < owner->container->inventory.size()) {
			return owner->container->inventory.at(itemIndex);
		}
	}
	return nullptr;
}

std::shared_ptr<Spell> playerAi::chooseFromSpells(std::shared_ptr<Entity> owner) {
	static const int frameW = 50, frameH = 28;
	static TCODConsole con(frameW, frameH);

	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, frameW, frameH, true, TCOD_BKGND_DEFAULT, "Spell List");

	int is = 'a';
	int i = 1;

	for(auto & spell : owner->spellCaster->spellList) {
		if(owner->spellCaster->getCharges(spell->getLevel()) == 0) con.setDefaultForeground(TCODColor::darkGrey);
		else con.setDefaultForeground(TCODColor::white);
		con.print(2, i, "(%c) %s (level %d)", is, spell->getName().c_str(), spell->getLevel());
		++i; ++is;
	}

	TCODConsole::blit(&con, 0, 0, frameW, frameH, TCODConsole::root, engine.screenWidth / 2 - frameW / 2, engine.screenHeight / 2 - frameH / 2);
	TCODConsole::flush();

	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr, true);
	if(key.vk == TCODK_CHAR) {
		int spellIndex = key.c - 'a';
		if(spellIndex >= 0 && spellIndex < owner->spellCaster->spellList.size()) {
			return owner->spellCaster->spellList.at(spellIndex);
		}
	}
	return nullptr;
}

TCOD_key_t playerAi::chooseInteractDirection() const {
	engine.ui->message(TCODColor::cyan, "Choose a direction to interact (Esc to cancel).");
	engine.ui->render();
	TCODConsole::flush();
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	return key;
}


static const int TRACK_TURNS(3);

mobAi::mobAi() : Ai() {}

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