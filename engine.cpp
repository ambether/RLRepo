#include "main.hpp"

Engine::Engine(int sW, int sH) : fovRadius(10), screenWidth(sW), screenHeight(sH), computeFov(true), gameState(START) {
	TCODConsole::initRoot(sW, sH, "memes to dreams", false);
	player = std::make_shared<Entity>(1, 1, "player", '@', TCODColor::white);
	player->mortal = std::make_shared<pcMortal>(30, 2, "your lifeless corpse");
	player->combat = std::make_shared<Combat>(5);
	player->ai = std::make_shared<playerAi>();
	player->container = std::make_shared<Container>(10);
	entityList.push_back(player);

	dungeon = std::make_unique<Map>(80, 43);
	gui = new Gui();
	gui->message(TCODColor::red, "Whalecum nerd.");
}

Engine::~Engine() { entityList.clear(); deadEntities.clear(); delete gui; }

std::shared_ptr<Entity> Engine::getClosestMonster(int x, int y, float range) const {
	std::shared_ptr<Entity> closest = NULL;
	float bestDist = 1E6f;
	for(auto & ent : entityList) {
		if(ent != player && ent->mortal) {
			if(!ent->mortal->isDead()) {
				float dist = ent->getDistance(x, y);
				if(dist < bestDist && (dist <= range || range == 0.0f)) { 
					bestDist = dist; 
					closest = ent;
				}
			}
		}
	}
	return closest;
}

std::shared_ptr<Entity> Engine::getMonster(int x, int y) const {
	for(auto & ent : entityList) {
		if(ent->x == x && ent->y == y) {
			if(ent->mortal) {
				if(!ent->mortal->isDead()) { return ent; }
			}
		}
	}
	return NULL;
}

bool Engine::pickTile(int * x, int * y, float maxRange) {
	while(!TCODConsole::isWindowClosed()) {
		render();
		for(int cx = 0; cx < dungeon->w; ++cx) {
			for(int cy = 0; cy < dungeon->h; ++cy) {
				if(dungeon->isInFov(cx, cy) && (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy); 
					col = col*1.2f;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
		if(dungeon->isInFov(mouse.cx, mouse.cy) && (maxRange == 0 || player->getDistance(mouse.cx, mouse.cy) <= maxRange)) {
			TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::black);
			if(mouse.lbutton_pressed) { *x = mouse.cx; *y = mouse.cy; return true; }
		}
		if(mouse.rbutton_pressed) { return false; }
		TCODConsole::flush();
	}
	return false;
}

void Engine::update() {
	if(gameState == START) dungeon->computeFov();
	gameState = IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
	player->update(player);
	if(gameState == TURN) {
		for(auto & ent : entityList) {
			if(ent->name != "player" && ent->mortal) {
				if(!ent->mortal->isDead()) { ent->update(ent); }
			}
		}
	}
	handleDeadEntities();
}

void Engine::render() {
	TCODConsole::root->clear();
	dungeon->render();
	for(auto & ent : entityList) { if(dungeon->isInFov(ent->x, ent->y)) { ent->render(); } }
	player->render();
	gui->render();
}

void Engine::notifyDeath(std::shared_ptr<Entity> entity) {
	deadEntities.emplace_back(entity);
}

void Engine::handleDeadEntities() {
	for(auto & ent : deadEntities) {
		std::deque<std::shared_ptr<Entity>>::iterator pos = std::find(entityList.begin(), entityList.end(), ent);
		if(pos != entityList.end()) {
			entityList.erase(pos);
			entityList.push_front(ent);
		}
	}
	deadEntities.clear();
}
