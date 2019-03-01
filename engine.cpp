#include "main.hpp"

Engine::Engine(int sW, int sH) : fovRadius(10), screenWidth(sW), screenHeight(sH), computeFov(true), gameState(START) {
	// Init the root console
	TCODConsole::initRoot(sW, sH, "memes to dreams", false);
	
	// Read from the data files
	DataFile * df = new DataFile();
	df->parseEntities();
	df->parseItems();
	df->parseSpells();

	// Insert the data into the template maps
	for(auto & ent : df->entList) {
		entityTemplates.insert(EntityMap::value_type(_strdup(ent->name), ent->clone()));
	}
	for(auto & it : df->itemList) {
		itemTemplates.insert(EntityMap::value_type(_strdup(it->name), it->clone()));
	}
	for(auto & sp : df->spellList) {
		spellTemplates.insert(SpellMap::value_type(_strdup(sp->name), std::make_shared<Spell>(*sp)));
	}
	delete df;
	
	// Initialize the Player
	player = entityTemplates["Player"]->clone();
	entityList.push_back(player); activeEntities.push_back(player);
	
	for(auto & pairs : spellTemplates) { // Add spells to Player
		player->spellCaster->spellList.push_back(std::make_shared<Spell>(*pairs.second));
	}

	// Make the dungeon
	dungeon = std::make_shared<Map>(130, 80);
	// Init the ui
	ui = std::make_shared<Ui>();
	ui->message(TCODColor::red, "Whalecum nerd.");
	
}

Engine::~Engine() {
	entityTemplates.clear();
	itemTemplates.clear();
	entityList.clear();
	activeEntities.clear();
	inactiveEntities.clear();
	deadEntities.clear(); 
	deaths.clear();
}

shared_ptr<Entity> Engine::getClosestMonster(int x, int y, float range) const {
	shared_ptr<Entity> closest = NULL;
	float bestDist = 1E6f;
	for(auto & ent : activeEntities) {
		if(ent != player) {
			float dist = ent->getDistance(x, y);
			if(dist < bestDist && (dist <= range || range == 0.0f)) {
				bestDist = dist;
				closest = ent;
			}
		}
	}
	return closest;
}

shared_ptr<Entity> Engine::getMonster(int x, int y) const {
	for(auto & ent : activeEntities) {
		if(ent->x == x && ent->y == y) { return ent; }
	}
	return nullptr;
}

bool Engine::pickTile(int * x, int * y, float maxRange, float radius) {
	while(!TCODConsole::isWindowClosed()) {
		render();
		int trueX, trueY;
		for(int cx = 0; cx < ui->viewport->getWidth(); ++cx) {
			for(int cy = 0; cy < ui->viewport->getHeight(); ++cy) {
				trueX = ui->viewport->getOffsetX() + cx;
				trueY = ui->viewport->getOffsetY() + cy;
				if(dungeon->isInFov(trueX, trueY) && (maxRange == 0 || player->getDistance(trueX, trueY) <= maxRange)) {
					if(radius > 0 && getDistance(cx, cy, mouse.cx, mouse.cy) <= radius) {
						TCODColor col = ui->viewport->getCharBackground(trueX, trueY);
						col = col * 0.8f;
						col.r += 80;
						ui->viewport->setCharBackground(trueX, trueY, col);
					}
					else {
						TCODColor col = ui->viewport->getCharBackground(trueX, trueY);
						col = col*1.2f;
						ui->viewport->setCharBackground(trueX, trueY, col);
					}
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
		int trueMouseX = mouse.cx + ui->viewport->getOffsetX();
		int trueMouseY = mouse.cy + ui->viewport->getOffsetY();
		if(dungeon->isInFov(trueMouseX, trueMouseY) && (maxRange == 0 || player->getDistance(trueMouseX, trueMouseY) <= maxRange)) {
			ui->viewport->setCharBackground(trueMouseX, trueMouseY, TCODColor::black);
			if(mouse.lbutton_pressed) { *x = trueMouseX; *y = trueMouseY; return true; }
		}
		if(mouse.rbutton_pressed) { return false; }
		ui->viewport->render();
		TCODConsole::flush();
	}
	return false;
}

void Engine::update() {
	if(gameState == START) dungeon->computeFov();
	if(gameState == NEW_TURN) {
		for(auto ent : activeEntities) {
			ent->gainEnergy();
		}
	}
	gameState = IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
	player->update(player);
	if(gameState == TURN) {
		for(auto & ent : activeEntities) {
			if(ent->name != "player") { ent->update(ent); }
		}
		actionQueue.processActions();
		handleDeadEntities();
		gameState = NEW_TURN;
	}
}

void Engine::render() {
	if(computeFov) { dungeon->computeFov(); computeFov = false; }
	TCODConsole::root->clear();
	ui->viewport->clear();
	ui->viewport->renderMap(dungeon);
	ui->viewport->moveView(player);
	for(auto & ent : deadEntities)		{ if(dungeon->isInFov(ent->x, ent->y)) { ui->viewport->renderEntity(ent); } }
	for(auto & ent : inactiveEntities)	{ if(dungeon->isInFov(ent->x, ent->y)) { ui->viewport->renderEntity(ent); } }
	for(auto & ent : activeEntities)	{ if(dungeon->isInFov(ent->x, ent->y)) { ui->viewport->renderEntity(ent); } }
	ui->render();
}

void Engine::notifyDeath(shared_ptr<Entity> entity) { deaths.emplace_back(entity); }

void Engine::addAction(shared_ptr<Action> action) { actionQueue.addAction(action); }

void Engine::setComputeFov(bool value) { computeFov = value; }

float Engine::getDistance(int x1, int y1, int x2, int y2) {
	int dx = x1 - x2;
	int dy = y1 - y2;
	return sqrtf(dx*dx + dy*dy);
}

void Engine::handleDeadEntities() {
	for(auto & ent : deaths) {
		auto pos = std::find(activeEntities.begin(), activeEntities.end(), ent);
		if(pos != activeEntities.end()) {
			ent->mortal->die(ent);
			activeEntities.erase(pos);
			deadEntities.push_back(ent);
		}
	}
	deaths.clear();
}

