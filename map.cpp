#include "main.hpp"

//static const int rsMIN = 6, rsMAX = 12;
//static const int rMonstersMAX = 3;
//static const int rItemsMAX = 3;

/*
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
*/

/*
Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width * height];
	map = std::make_shared<TCODMap>(width, height);
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(NULL, 8, rsMAX, rsMAX, 1.5f, 1.5f);
	bspList listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, NULL);
	placeDoors();
}
*/

Map::Map(int width, int height) : width(width), height(height) {
	exploredTiles = new Tile[width * height];
	map = std::make_shared<TCODMap>(width, height);
	std::random_device seed;
	rng = std::make_shared<TCODRandom>(seed());
	generateMap(); // ??????
}

Map::~Map() { delete [] exploredTiles; }

int Map::getWidth() const { return width; }

int Map::getHeight() const { return height; }

bool Map::canWalk(int x, int y) const {
	if(isWall(x, y)) return false;
	for(auto & ent : engine.entityList) { if(ent->blocks && ent->x == x && ent->y == y) { return false; } }
	return true;
}

bool Map::isWall(int x, int y) const { return !map->isWalkable(x, y); }

bool Map::isExplored(int x, int y) const { return exploredTiles[x + y*width].isExplored; }

bool Map::isInFov(int x, int y) const {
	if(x < 0 || x >= width || y < 0 || y >= height) { return false; }
	if(map->isInFov(x, y)) { exploredTiles[x + y * width].isExplored = true; return true; }
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

void Map::setTransparent(int x, int y, bool transparent) { map->setProperties(x, y, transparent, map->isWalkable(x, y)); }

void Map::generateMap() {
	// Make initial room
	createRoom(width / 2, height / 2, EAST);

	int currentFeatures = 1; // Starts at 1 because of the initial room

	while(currentFeatures < MAX_FEATURES) {
		int newx = 0, newy = 0;
		Direction dir = NONE;
		for(int testing = 0; testing < 1000; ++testing) { // Test random tiles to find a place to put a new feature
			newx = rng->getInt(0, width - 1);
			newy = rng->getInt(0, height - 1);
			dir = NONE;
			// Try to find a non-walkable tile with an adjacent walkable tile
			if(canWalk(newx, newy)) continue; // Go back to the top of the loop
			if(canWalk(newx, newy + 1)) {
				if(createRoom(newx, newy - 1, NORTH)) { 
					++currentFeatures; 
					map->setProperties(newx, newy, true, true); // change to door later
					map->setProperties(newx, newy - 1, true, true);
					break;
				}
			}
			else if(canWalk(newx - 1, newy)) {
				if(createRoom(newx + 1, newy, EAST)) { 
					++currentFeatures;
					map->setProperties(newx, newy, true, true); // change to door later
					map->setProperties(newx + 1, newy, true, true);
					break; 
				}
			}
			else if(canWalk(newx, newy - 1)) {
				if(createRoom(newx, newy + 1, SOUTH)) { 
					++currentFeatures;
					map->setProperties(newx, newy, true, true); // change to door later
					map->setProperties(newx, newy + 1, true, true);
					break; 
				}
			}
			else if(canWalk(newx + 1, newy)) {
				if(createRoom(newx - 1, newy, WEST)) { 
					++currentFeatures;
					map->setProperties(newx, newy, true, true); // change to door later
					map->setProperties(newx - 1, newy, true, true);
					break; 
				}
			}
		}
	}
}

void Map::dig(const Rectangle & rect) {
	for(int x = rect.x; x < rect.x + rect.width; ++x) {
		for(int y = rect.y; y < rect.y + rect.height; ++y) {
			map->setProperties(x, y, true, true);
		}
	}
}

bool Map::canPutRoom(const Rectangle & room) const {
	if(room.x < 1 || room.y < 1 || room.x + room.width > width 
		|| room.y + room.height > height - 1) { // Check if room is out of bounds
		return false;
	}
	for(int y = room.y; y < room.y + room.height; ++y) {
		for(int x = room.x; x < room.x + room.width; ++x) {
			if(canWalk(x, y)) { return false; } // Check if this room will overlap an existing room
		}
	}
	return true;
}

bool Map::createRoom(int x, int y, Direction dir) {
	Rectangle room;

	room.width = rng->getInt(ROOM_SIZE_MIN, ROOM_SIZE_MAX);
	room.height = rng->getInt(ROOM_SIZE_MIN, ROOM_SIZE_MAX);
	room.x = x;
	room.y = y;

	switch(dir) {
	case NORTH:
	{	
		room.x = x - room.width / 2;
		room.y = y - room.height;
		break;
	}
	case EAST:
	{
		room.x = x + 1;
		room.y = y - room.height / 2;
		break;
	}
	case SOUTH:
	{
		room.x = x - room.width / 2;
		room.y = y + 1;
		break;
	}
	case WEST:
	{
		room.x = x - room.width;
		room.y = y - room.height / 2;
		break;
	}
	}

	if(canPutRoom(room)) { 
		dig(room); 
		return true; // Room successfully created
	}
	return false; // Room was not created
}

/*
void Map::addMonster(int x, int y) {
	std::random_device seed;
	shared_ptr<TCODRandom> rng = std::make_shared<TCODRandom>(seed());
	int roll = rng->getInt(1, 100);
	
	const char * monsterName = "";

	if(roll < 80) {	monsterName = "Gobbo"; }
	else { monsterName = "Hobbo"; }

	auto it = engine.entityTemplates.find(monsterName); // Try to find a template for monsterName.
	if(it != engine.entityTemplates.end()) {
		shared_ptr<Entity> monster = it->second->clone();
		monster->x = x;
		monster->y = y;
		engine.entityList.push_back(monster);
		engine.activeEntities.push_back(monster);
	}
}

void Map::addDoor(int x, int y) {
	shared_ptr<Entity> door = std::make_shared<Entity>(x, y, "door", '+', TCODColor::darkAmber);
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
*/


/*
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
	shared_ptr<TCODRandom> rng = std::make_shared<TCODRandom>(seed());
	int roll = rng->getInt(1, 100);

	const char * itemName = "";

	if(roll < 70) {	itemName = "Healing Potion"; }
	else if(roll < 70 + 10) { itemName = "Lightning Bolt Scroll"; }
	else if(roll < 70 + 10 + 10) { itemName = "Fireball Scroll"; }
	
	auto it = engine.itemTemplates.find(itemName); // Try to find a template for itemName.
	if(it != engine.itemTemplates.end()) {
		shared_ptr<Entity> item = it->second->clone();
		item->x = x;
		item->y = y;
		engine.entityList.push_back(item);
		engine.inactiveEntities.push_back(item);
	}
}
*/