#include "main.hpp"

Engine::Engine(int sW, int sH) : fovRadius(10), screenWidth(sW), screenHeight(sH), computeFov(true), gameState(START) {
	TCODConsole::initRoot(sW, sH, "memes to dreams", false);
	player = std::make_shared<Entity>(1, 1, "player", '@', TCODColor::white);
	player->mortal = std::make_shared<pcMortal>(30, 2, "your lifeless corpse");
	player->combat = std::make_shared<Combat>(5);
	player->ai = std::make_shared<playerAi>();
	player->container = std::make_shared<Container>(10);
	player->spellCaster = std::make_shared<SpellCaster>();
	player->spellCaster->spellList.push_back(std::make_shared<DamageSpell>("Magic Missle", 1, 10.0f, 5.0f, TCODColor::lighterPurple));
	player->spellCaster->spellList.push_back(std::make_shared<DamageSpell>("Fireball", 2, 5.0f, 2.0f, 8.0f, TCODColor::flame));
	entityList.push_back(player); activeEntities.push_back(player);

	dungeon = std::make_shared<Map>(130, 65);
	gui = std::make_shared<Gui>();
	gui->message(TCODColor::red, "Whalecum nerd.");
	viewport = std::make_shared<Viewport>(dungeon->getWidth(), dungeon->getHeight(), screenWidth, screenHeight - gui->height);
}

Engine::~Engine() {
	entityList.clear();
	activeEntities.clear();
	inactiveEntities.clear();
	deadEntities.clear(); 
	deaths.clear();
}

std::shared_ptr<Entity> Engine::getClosestMonster(int x, int y, float range) const {
	std::shared_ptr<Entity> closest = NULL;
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

std::shared_ptr<Entity> Engine::getMonster(int x, int y) const {
	for(auto & ent : activeEntities) {
		if(ent->x == x && ent->y == y) { return ent; }
	}
	return nullptr;
}

bool Engine::pickTile(int * x, int * y, float maxRange, float radius) {
	while(!TCODConsole::isWindowClosed()) {
		render();
		int trueX, trueY;
		for(int cx = 0; cx < viewport->getViewWidth(); ++cx) {
			for(int cy = 0; cy < viewport->getViewHeight(); ++cy) {
				trueX = viewport->getOffsetX() + cx;
				trueY = viewport->getOffsetY() + cy;
				if(dungeon->isInFov(trueX, trueY) && (maxRange == 0 || player->getDistance(trueX, trueY) <= maxRange)) {
					if(radius > 0 && getDistance(cx, cy, mouse.cx, mouse.cy) <= radius) {
						TCODColor col = viewport->getCharBackground(trueX, trueY);
						col = col * 0.8f;
						col.r += 80;
						viewport->setCharBackground(trueX, trueY, col);
					}
					else {
						TCODColor col = viewport->getCharBackground(trueX, trueY);
						col = col*1.2f;
						viewport->setCharBackground(trueX, trueY, col);
					}
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
		int trueMouseX = mouse.cx + viewport->getOffsetX();
		int trueMouseY = mouse.cy + viewport->getOffsetY();
		if(dungeon->isInFov(trueMouseX, trueMouseY) && (maxRange == 0 || player->getDistance(trueMouseX, trueMouseY) <= maxRange)) {
			viewport->setCharBackground(trueMouseX, trueMouseY, TCODColor::black);
			if(mouse.lbutton_pressed) { *x = trueMouseX; *y = trueMouseY; return true; }
		}
		if(mouse.rbutton_pressed) { return false; }
		viewport->render();
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
	viewport->clear();
	viewport->renderMap(dungeon);
	viewport->moveView(player);
	for(auto & ent : deadEntities) { if(dungeon->isInFov(ent->x, ent->y)) { viewport->renderEntity(ent); } }
	for(auto & ent : inactiveEntities) { if(dungeon->isInFov(ent->x, ent->y)) { viewport->renderEntity(ent); } }
	for(auto & ent : activeEntities) { if(dungeon->isInFov(ent->x, ent->y)) { viewport->renderEntity(ent); } }
	viewport->render();
	gui->render();
}

void Engine::notifyDeath(std::shared_ptr<Entity> entity) { deaths.emplace_back(entity); }

void Engine::addAction(std::shared_ptr<Action> action) { actionQueue.addAction(action); }

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

