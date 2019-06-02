#include "main.hpp"

Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width * height];
	map = std::make_shared<TCODMap>(width, height);
	std::random_device seed;
	rng = std::make_shared<TCODRandom>(seed());
	generateMap();
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
	if(map->isInFov(x, y)) { getTile(x, y).isExplored = true; return true; }
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

Tile & Map::getTile(int x, int y) const { return tiles[x + y*width]; }

void Map::generateMap() {
	// Make initial room
	createFeature(width / 2, height / 2, Direction(rng->getInt(1, 4)), Tile::ROOM); // A random int from 1-4 gives you N/S/E/W from Direction

	int currentFeatures = 1; // Starts at 1 because of the initial room

	for(currentFeatures; currentFeatures < MAX_FEATURES; ++currentFeatures) {
		generateFeature();
	}
}

// Digs out tiles in a rectangle and sets them to the appropriate TileType
void Map::dig(const Rectangle & rect, Tile::TileType tileType) { 
	for(int x = rect.x; x < rect.x + rect.width; ++x) {
		for(int y = rect.y; y < rect.y + rect.height; ++y) {
			map->setProperties(x, y, true, true); // Makes the square walkable and transparent
			tiles[x + y*width].tileType = tileType; // Sets the TileType
		}
	}
}

void Map::createDoor(int x, int y) {
	shared_ptr<Entity> door = engine.entityTemplates["Door"]->clone();
	door->x = x;
	door->y = y;
	setTransparent(x, y, false);
	engine.entityList.push_back(door); engine.inactiveEntities.push_back(door);
}

// Checks if a room/corridor/etc is able to be placed without going out of bounds or overlapping other features
bool Map::canPutFeature(const Rectangle & rect) const {
	if(rect.x < 1 || rect.y < 1 || rect.x + rect.width > width 
		|| rect.y + rect.height > height - 1) { // Check if rect is out of bounds
		return false;
	}
	for(int y = rect.y; y < rect.y + rect.height; ++y) {
		for(int x = rect.x; x < rect.x + rect.width; ++x) {
			if(canWalk(x, y)) { return false; } // Check if this rect will overlap an existing rect
		}
	}
	return true;
}

void Map::generateFeature(Rectangle * bounds) {
	int newx = 0, newy = 0;
	Direction dir = NONE;
	for(int testing = 0; testing < 1000; ++testing) { // Test random tiles to find a place to put a new feature
		if(bounds) { // If a bounds rectangle is defined, only search within those bounds
			newx = rng->getInt(bounds->x - 1, bounds->x + bounds->width);
			newy = rng->getInt(bounds->y - 1, bounds->y + bounds->height);
		}
		else {
			newx = rng->getInt(1, width - 1);
			newy = rng->getInt(1, height - 1);
		}
		dir = NONE;
		// Try to find a wall or corridor tile
		if(getTile(newx, newy).tileType == Tile::ROOM) continue; // Start over if the tile is a room, otherwise it's a wall or corridor
		
		if(getTile(newx, newy + 1).tileType == Tile::ROOM || getTile(newx, newy + 1).tileType == Tile::CORRIDOR) {
			if(createFeature(newx, newy - 1, NORTH)) {
				map->setProperties(newx, newy, true, true); // Clear a path to the new feature
				createDoor(newx, newy); // Add a door
				map->setProperties(newx, newy - 1, true, true); // Clean in front of door
				break;
			}
		}
		else if(getTile(newx - 1, newy).tileType == Tile::ROOM || getTile(newx - 1, newy).tileType == Tile::CORRIDOR) {
			if(createFeature(newx + 1, newy, EAST)) {
				map->setProperties(newx, newy, true, true); // Clear a path to the new feature
				createDoor(newx, newy); // Add a door
				map->setProperties(newx + 1, newy, true, true); // Clean in front of door
				break;
			}
		}
		else if(getTile(newx, newy - 1).tileType == Tile::ROOM || getTile(newx, newy - 1).tileType == Tile::CORRIDOR) {
			if(createFeature(newx, newy + 1, SOUTH)) {
				map->setProperties(newx, newy, true, true); // Clear a path to the new feature
				createDoor(newx, newy); // Add a door
				map->setProperties(newx, newy + 1, true, true); // Clean in front of door
				break;
			}
		}
		else if(getTile(newx + 1, newy).tileType == Tile::ROOM || getTile(newx + 1, newy).tileType == Tile::CORRIDOR) {
			if(createFeature(newx - 1, newy, WEST)) {
				map->setProperties(newx, newy, true, true); // Clear a path to the new feature
				createDoor(newx, newy); // Add a door
				map->setProperties(newx - 1, newy, true, true); // Clean in front of door
				break;
			}
		}
	}
}

bool Map::createFeature(int x, int y, Direction dir, Tile::TileType tileType) {
	const static int ROOM_CHANCE = 85;
	int chance = 0;

	if(tileType == Tile::ROOM) 
		chance = 1;
	else if(tileType == Tile::CORRIDOR) 
		chance = ROOM_CHANCE + 1;
	else 
		chance = rng->getInt(1, 100);

	if(chance <= ROOM_CHANCE) {
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

		if(canPutFeature(room)) {
			dig(room, Tile::ROOM);
			return true; // Room successfully created
		}
	}
	else { // Corridor (for now)
		Rectangle corridor;
		corridor.x = x;
		corridor.y = y;
		int length = rng->getInt(ROOM_SIZE_MIN, ROOM_SIZE_MAX);
		switch(dir) {
		case NORTH:
		{
			corridor.width = 1;
			corridor.height = length;
			corridor.y = y - length;
			break;
		}
		case EAST:
		{
			corridor.width = length;
			corridor.height = 1;
			corridor.x = x + 1;
			break;
		}
		case SOUTH:
		{
			corridor.height = length;
			corridor.width = 1;
			corridor.y = y + 1;
			break;
		}
		case WEST:
		{
			corridor.width = length;
			corridor.height = 1;
			corridor.x = x - length;
		}
		}

		if(canPutFeature(corridor)) {
			dig(corridor, Tile::CORRIDOR);
			generateFeature(&corridor); // If a corridor is created, add a feature to it (a corridor would be made to go somewhere!)
			return true; // Corridor successfully created
		}
	}
	return false;
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