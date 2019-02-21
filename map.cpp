#include "main.hpp"

static const int rsMIN = 6, rsMAX = 12;
static const int rMonstersMAX = 3;
static const int rItemsMAX = 3;

class bspList : public ITCODBspCallback {
private:
	Map & dungeon;
	int rNum;
	int lastX, lastY;
public:
	bspList(Map & dungeon) : dungeon(dungeon), rNum(0) {}
	bool visitNode(TCODBsp * node, void * userData) {
		if(node->isLeaf()) {
			int x, y, w, h;
			TCODRandom * rng = TCODRandom::getInstance();
			w = rng->getInt(rsMIN, node->w - 2);
			h = rng->getInt(rsMIN, node->h - 2);
			x = rng->getInt(node->x + 1, node->x + node->w - w - 1);
			y = rng->getInt(node->y + 1, node->y + node->h - h - 1);
			dungeon.createRoom(rNum == 0, x, y, x + w - 1, y + h - 1);
			if(rNum != 0) {
				dungeon.dig(lastX, lastY, x + w / 2, lastY); // Horizontal tunnel
				dungeon.dig(x + w / 2, lastY, x + w / 2, y + h / 2); // Vertical tunnel

			}
			lastX = x + w / 2;
			lastY = y + h / 2;
			++rNum;
		}
		return true;
	}
};

Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width * height];
	map = std::make_shared<TCODMap>(width, height);
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(NULL, 8, rsMAX, rsMAX, 1.5f, 1.5f);
	bspList listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, NULL);
	placeDoors();
}

Map::~Map() { delete [] tiles; }

int Map::getWidth() const { return width; }

int Map::getHeight() const { return height; }

bool Map::canWalk(int x, int y) const {
	if(isWall(x, y)) return false;
	for(auto & ent : engine.entityList) { if(ent->blocks && ent->x == x && ent->y == y) { return false; } }
	return true;
}

bool Map::isWall(int x, int y) const { return !map->isWalkable(x, y); }

bool Map::isExplored(int x, int y) const { return tiles[x + y*width].isExplored; }

bool Map::isInFov(int x, int y) const {
	if(x < 0 || x >= width || y < 0 || y >= height) { return false; }
	if(map->isInFov(x, y)) { tiles[x + y * width].isExplored = true; return true; }
	return false;
}

void Map::computeFov() { map->computeFov(engine.player->x, engine.player->y, engine.fovRadius); }

void Map::render(TCODConsole * renderConsole) const {
	static const TCODColor darkWall(0, 0, 30),
		darkFloor(50, 50, 80),
		lightWall(80, 80, 110),
		lightFloor(100, 100, 100);
	
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(isInFov(x, y)) { renderConsole->setCharBackground(x, y, isWall(x, y) ? lightWall : lightFloor); }
			else if(isExplored(x, y)) { renderConsole->setCharBackground(x, y, isWall(x, y) ? darkWall : darkFloor); }
		}
	}
}

void Map::addMonster(int x, int y) {
	std::random_device seed;
	std::shared_ptr<TCODRandom> rng = std::make_shared<TCODRandom>(seed());
	int roll = rng->getInt(1, 100);
	if(roll < 80) {
		std::shared_ptr<Entity> gobbo = std::make_shared<Entity>(x, y, "Gobbo", 'g', TCODColor::desaturatedGreen);
		gobbo->mortal = std::make_shared<Mortal>(15, "dead gobbo");
		gobbo->combat = std::make_shared<Combat>(1, 1, 5);
		gobbo->ai = std::make_shared<mobAi>(50);
		engine.entityList.push_back(gobbo); engine.activeEntities.push_back(gobbo);
	}
	else {
		std::shared_ptr<Entity> hobbo = std::make_shared<Entity>(x, y, "Hobbo", 'h', TCODColor::darkOrange);
		hobbo->mortal = std::make_shared<Mortal>(20, "dead hobbo");
		hobbo->combat = std::make_shared<Combat>(4, 3, 6); 
		hobbo->ai = std::make_shared<mobAi>(150);
		engine.entityList.push_back(hobbo); engine.activeEntities.push_back(hobbo);
	}
}

void Map::addDoor(int x, int y) {
	std::shared_ptr<Entity> door = std::make_shared<Entity>(x, y, "door", '+', TCODColor::darkAmber);
	door->interaction = std::make_shared<DoorInteraction>();
	setTransparent(x, y, false);
	engine.entityList.push_back(door); engine.inactiveEntities.push_back(door);
}

// Checks the edges of the rooms for gaps and only places a
// door in a non-wall on the edge if it has a wall on either side of it.
void Map::placeDoors() {
	for(int i = 0; i < rooms.size(); ++i) {
		RoomData rd = rooms[i];

		bool wallLastTop = true, wallLastBottom = true;
		bool wallCurrentTop = false, wallCurrentBottom = false;
		for(int x = rd.x; x < rd.x + rd.w; ++x) { // Iterate over the horizontal edges
			// Check the top edge
			wallCurrentTop = isWall(x, rd.y - 1);
			if(wallCurrentTop == false && wallLastTop == true) {
				// Only add a door if the next space on the edge is a wall
				if(isWall(x + 1, rd.y - 1)) { addDoor(x, rd.y - 1); }
			}

			// Check the bottom edge
			wallCurrentBottom = isWall(x, rd.y + rd.h);
			if(wallCurrentBottom == false && wallLastBottom == true) {
				// Only add a door if the next space on the edge is a wall
				if(isWall(x + 1, rd.y + rd.h)) { addDoor(x, rd.y + rd.h); }
			}
			wallLastTop = wallCurrentTop;
			wallLastBottom = wallCurrentBottom;
		}
		
		bool wallLastLeft = true, wallLastRight = true;
		bool wallCurrentLeft = false, wallCurrentRight = false;
		for(int y = rd.y; y < rd.y + rd.h; ++y) { // Iterate over the vertical edges
			// Check the left edge
			wallCurrentLeft = isWall(rd.x - 1, y);
			if(wallCurrentLeft == false && wallLastLeft == true) {
				// Only add a door if the next space on the edge is a wall
				if(isWall(rd.x - 1, y + 1)) { addDoor(rd.x - 1, y); } 
			}

			// Check the right edge
			wallCurrentRight = isWall(rd.x + rd.w, y);
			if(wallCurrentRight == false && wallLastRight == true) {
				// Only add a door if the next space on the edge is a wall
				if(isWall(rd.x + rd.w, y + 1)) { addDoor(rd.x + rd.w, y); } 
			}
			wallLastLeft = wallCurrentLeft;
			wallLastRight = wallCurrentRight;
		}
	}
}

void Map::setTransparent(int x, int y, bool transparent) { map->setProperties(x, y, transparent, map->isWalkable(x, y)); }

void Map::dig(int x1, int y1, int x2, int y2) {
	if(x2 < x1) { int tmp = x2; x2 = x1; x1 = tmp; }
	if(y2 < y1) { int tmp = y2; y2 = y1; y1 = tmp; }
	for(int tx = x1; tx <= x2; ++tx) {
		for(int ty = y1; ty <= y2; ++ty) {
			map->setProperties(tx, ty, true, true);
		}
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
	rooms.push_back(RoomData(x1, y1, x2 - x1 + 1, y2 - y1 + 1));
	dig(x1, y1, x2, y2);
	int cx = (x1 + x2) / 2;
	int cy = (y1 + y2) / 2;
	
	if(first) {
		engine.player->x = cx;
		engine.player->y = cy;
	} 
	else {
		TCODRandom * rng = TCODRandom::getInstance();
		int nbMonsters = rng->getInt(0, rMonstersMAX);
		while(nbMonsters > 0) {
			int x = rng->getInt(x1, x2),
				y = rng->getInt(y1, y2);
			if(canWalk(x, y)) { addMonster(x, y); }
			--nbMonsters;
		}
		int nbItems = rng->getInt(0, rItemsMAX);
		while(nbItems > 0) {
			int x = rng->getInt(x1, x2),
				y = rng->getInt(y1, y2);
			if(canWalk(x, y)) { addItem(x, y); }
			--nbItems;
		}
	}
}

void Map::addItem(int x, int y) {
	std::random_device seed;
	std::shared_ptr<TCODRandom> rng = std::make_shared<TCODRandom>(seed());
	int roll = rng->getInt(1, 100);
	if(roll < 70) {
		std::shared_ptr<Entity> healPotion = std::make_shared<Entity>(x, y, "Healing Potion", '!',  TCODColor::crimson);
		healPotion->blocks = false; healPotion->loot = std::make_shared<Healer>(4);
		engine.entityList.push_back(healPotion); engine.inactiveEntities.push_back(healPotion);
	}
	else if(roll < 70 + 10) {
		std::shared_ptr<Entity> lBoltScroll = std::make_shared<Entity>(x, y, "Lightning Bolt Scroll", '#', TCODColor::azure);
		lBoltScroll->blocks = false; lBoltScroll->loot = std::make_shared<LightningBolt>(5.0f, 20);
		engine.entityList.push_back(lBoltScroll); engine.inactiveEntities.push_back(lBoltScroll);
	}
	else if(roll < 70 + 10 + 10) {
		std::shared_ptr<Entity> fBallScroll = std::make_shared<Entity>(x, y, "Fireball Scroll", '#',  TCODColor::flame);
		fBallScroll->blocks = false; fBallScroll->loot = std::make_shared<Fireball>(8.0f, 12, 3.0f);
		engine.entityList.push_back(fBallScroll); engine.inactiveEntities.push_back(fBallScroll);
	}
	else {
		//std::shared_ptr<Ent> ConfusionScroll = std::make_shared<Ent>(x, y, '#', "Confusion Scroll", TCODColor::lightGreen);
		//ConfusionScroll->blocks = false; ConfusionScroll->loot = std::make_shared<Confuser>(10, 8);
		//engine.entL.push_back(ConfusionScroll);
	}
}