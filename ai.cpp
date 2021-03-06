#include "main.hpp"

Ai::Ai() : speed(0), energy(0) {}

Ai::Ai(int speed) : speed(speed), energy(0) {}

void Ai::gainEnergy() { energy += speed; }

void Ai::setSpeed(int speed) { this->speed = speed; }

int Ai::getSpeed() const { return speed; }

void Ai::spendEnergy() { if(energy >= 100) energy -= 100; }


playerAi::playerAi() : Ai(100) {}

playerAi::playerAi(const playerAi & obj) {
	speed = obj.speed;
	energy = obj.energy;
}

shared_ptr<Ai> playerAi::clone() const {
	return std::make_shared<playerAi>(*this);
}

void playerAi::update(shared_ptr<Entity> owner) {
	if(owner->mortal && !owner->mortal->isDead()) {
		if(energy >= 100) {
			int dx = 0, dy = 0;
			InputCommandType input = handleInputCommand(engine.lastKey.vk);
			switch(input) {
			case MOVE_N:  dy = -1; break;
			case MOVE_S:  dy =  1; break;
			case MOVE_W:  dx = -1; break;
			case MOVE_E:  dx =  1; break;
			case MOVE_NW: dx = -1; dy = -1; break;
			case MOVE_NE: dx =  1; dy = -1; break;
			case MOVE_SW: dx = -1; dy =  1; break;
			case MOVE_SE: dx =  1; dy =  1; break;
			case IDLE: handleActionKey(owner, '.'); break;
			case CHAR: handleActionKey(owner, engine.lastKey.c); break;
			case NULL_INPUT:
			default: break;
			}
			if(dx != 0 || dy != 0) {
				if(moveOrAttack(owner, dx, dy)) { engine.dungeon->computeFov(); } // only returns true if it moves the Entity
			}
		}
		if(energy < 100) engine.gameState = Engine::TURN;
	}
}

void playerAi::handleActionKey(shared_ptr<Entity> owner, int ascii) {
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
			shared_ptr<Entity> item = chooseFromInv(owner);
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
			InputCommandType input = handleInputCommand(key.vk);
			switch(input) {
			case MOVE_N:  dy = -1; break;
			case MOVE_S:  dy =  1; break;
			case MOVE_W:  dx = -1; break;
			case MOVE_E:  dx =  1; break;
			case MOVE_NW: dx = -1; dy = -1; break;
			case MOVE_NE: dx =  1; dy = -1; break;
			case MOVE_SW: dx = -1; dy =  1; break;
			case MOVE_SE: dx =  1; dy =  1; break;
			case IDLE:
			case CHAR:
			case NULL_INPUT:
			default: engine.ui->message(TCODColor::grey, "Canceled."); return;
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
			shared_ptr<Spell> spell = chooseFromSpells(owner);
			if(spell != nullptr && spell->canUse(owner)) {
				spendEnergy();
				engine.addAction(std::make_shared<CastAction>(spell, owner));
			}
		}
		break;
	}
}

bool playerAi::moveOrAttack(shared_ptr<Entity> owner, int dx, int dy) {
	if(engine.dungeon->isWall(owner->x + dx, owner->y + dy)) { return false; }
	spendEnergy();
	engine.addAction(std::make_shared<MoveAction>(owner, dx, dy));
	return true;
}

shared_ptr<Entity> playerAi::chooseFromInv(shared_ptr<Entity> owner) {
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

shared_ptr<Spell> playerAi::chooseFromSpells(shared_ptr<Entity> owner) {
	static const int frameW = 50, frameH = 28;
	static TCODConsole con(frameW, frameH);

	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, frameW, frameH, true, TCOD_BKGND_DEFAULT, "Spell List");

	int is = 'a';
	int i = 1;

	for(auto & spell : owner->spellCaster->spellList) {
		if(owner->spellCaster->getCharges(spell->level) == 0) con.setDefaultForeground(TCODColor::darkGrey);
		else con.setDefaultForeground(TCODColor::white);
		con.print(2, i, "(%c) %s (level %d)", is, spell->name, spell->level);
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
	engine.ui->message(TCODColor::white, "Choose a direction to interact (Esc to cancel).");
	engine.ui->render();
	TCODConsole::flush();
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	return key;
}

playerAi::InputCommandType playerAi::handleInputCommand(TCOD_keycode_t key) {
	switch(key) {
	case TCODK_UP:					// Up
	case TCODK_KP8: return MOVE_N;

	case TCODK_DOWN:				// Down
	case TCODK_KP2: return MOVE_S;
	
	case TCODK_LEFT:				// Left
	case TCODK_KP4: return MOVE_W;
	
	case TCODK_RIGHT:				// Right
	case TCODK_KP6: return MOVE_E;

	case TCODK_KP7: return MOVE_NW;	// Up-Left

	case TCODK_KP9: return MOVE_NE;	// Up-Right

	case TCODK_KP1: return MOVE_SW;	// Down-Left

	case TCODK_KP3: return MOVE_SE;	// Down-Right

	case TCODK_KP5: return IDLE;	// Idle

	case TCODK_CHAR: return CHAR;	// Char

	default: return NULL_INPUT;		// Nothing
	}
}


static const int TRACK_TURNS(3);

mobAi::mobAi() : Ai() {}

mobAi::mobAi(int speed) : Ai(speed) {}

mobAi::mobAi(const mobAi & obj) {
	speed = obj.speed;
	energy = obj.energy;
	moveCount = obj.moveCount;
}

shared_ptr<Ai> mobAi::clone() const {
	return std::make_shared<mobAi>(*this);
}

void mobAi::update(shared_ptr<Entity> owner) {
	if(owner->mortal && owner->mortal->isDead()) { return; }

	while(energy >= 100) {
		if(engine.dungeon->isInFov(owner->x, owner->y)) { moveCount = TRACK_TURNS; }
		else { --moveCount; }

		if(moveCount > 0) {	moveOrAttack(owner); }
		else { idle(owner); }
	}
}

void mobAi::moveOrAttack(shared_ptr<Entity> owner) {
	spendEnergy();
	engine.addAction(std::make_shared<MoveAtPlayerAction>(owner));
}

void mobAi::idle(shared_ptr<Entity> owner) {
	spendEnergy();
	engine.addAction(std::make_shared<IdleAction>(owner));
}