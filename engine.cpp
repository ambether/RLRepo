#include "main.hpp"

Engine::Engine(int sW, int sH) : fovRadius(10), screenWidth(sW), screenHeight(sH), computeFov(true), gameState(START) {
	TCODConsole::initRoot(sW, sH, "memes to dreams", false);
	player = std::make_shared<Entity>(1, 1, "Player", '@', TCODColor::white);
	player->mortal = std::make_shared<pcMortal>(30, "your lifeless corpse");
	player->combat = std::make_shared<Combat>(1, 3, 10);
	player->ai = std::make_shared<playerAi>();
	player->container = std::make_shared<Container>(10);
	player->spellCaster = std::make_shared<SpellCaster>();
	player->spellCaster->spellList.push_back(std::make_shared<DamageSpell>("Magic Missle", 1, 10.0f, 5, TCODColor::lighterPurple));
	player->spellCaster->spellList.push_back(std::make_shared<DamageSpell>("Fireball", 2, 5.0f, 2.0f, 8, TCODColor::flame));
	entityList.push_back(player); activeEntities.push_back(player);

	dungeon = std::make_shared<Map>(130, 80);
	ui = std::make_shared<Ui>();
	ui->message(TCODColor::red, "Whalecum nerd.");

	DataFile * df = new DataFile();
	df->parseEntities();
	df->parseItems();
	delete df;
}

Engine::~Engine() {
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

