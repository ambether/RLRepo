#include "main.hpp"

Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width * height];
	rooms = vector<Rectangle>();
	map = std::make_shared<TCODMap>(width, height);
	std::random_device seed;
	rng = std::make_shared<TCODRandom>(seed());
	currentFeatures = 0;
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

bool Map::isExplored(int x, int y) const { return getTile(x, y).isExplored; }

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

Map::Tile & Map::getTile(int x, int y) const { return tiles[x + y*width]; }

void Map::generateMap() {
	// Make initial room
	createFeature(width / 2, height / 2, Direction(rng->getInt(1, 4)), Tile::ROOM); // A random int from 1-4 gives you N/S/E/W from Direction

	currentFeatures = 1; // Starts at 1 because of the initial room

	// Generate features until the limit is reached
	while(currentFeatures <= MAX_FEATURES) { generateFeature(); }

	// Place the Entities on the Map
	placeEntities();
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
	engine.entityList.push_back(door);
	engine.inactiveEntities.push_back(door);
}

// Places the Player and randomly adds monsters to rooms
void Map::placeEntities() {
	const static int MONSTER_CHANCE = 65,
		MAX_MONSTERS = 5;

	Rectangle room = rooms[0];
	engine.player->x = room.x + room.width / 2;
	engine.player->y = room.y + room.height / 2;

	for(int i = 1; i < rooms.size(); ++i) { // Iterate through all rooms other than the first
		room = rooms[i];
		int chance = rng->getInt(1, 100); // Decide randomly to add monsters to the room
		if(chance <= MONSTER_CHANCE) {
			int numMonsters = rng->getInt(1, MAX_MONSTERS); // Decide on a random number of monsters to place
			int x, y;
			for(int i = 0; i < numMonsters; ++i) { // Add monsters at random coordinates inside the room
				x = rng->getInt(room.x, room.x + room.width - 1);
				y = rng->getInt(room.y, room.y + room.height - 1);
				addMonster(x, y);
			}
		}
	}
}

// Adds a random monster at the given x, y
void Map::addMonster(int x, int y) {
	const static int GOBBO_CHANCE = 70, HOBBO_CHANCE = 30;

	const char * monsterName = "";

	int chance = rng->getInt(1, 100);
	if(chance <= GOBBO_CHANCE) { // chance <= 70
		monsterName = "Gobbo";
	}
	else if(chance <= GOBBO_CHANCE + HOBBO_CHANCE) { // 70 < chance <= 70 + 30
		monsterName = "Hobbo";
	}
	std::shared_ptr<Entity> monster = engine.entityTemplates[monsterName]->clone();
	monster->x = x;
	monster->y = y;
	engine.entityList.push_back(monster);
	engine.activeEntities.push_back(monster);
}

void Map::generateFeature(Rectangle * bounds) {
	int newx = 0, newy = 0;
	Direction dir = NONE;
	
	for(int testing = 0; testing < 1000; ++testing) { // Test random tiles to find a place to put a new feature
		if(bounds) { // If a bounds rectangle is defined, only search within those bounds
			newx = rng->getInt(bounds->x - 1, bounds->x + bounds->width);
			newy = rng->getInt(bounds->y - 1, bounds->y + bounds->height);
		}
		else { // Pick a random spot on the Map
			newx = rng->getInt(1, width - 1);
			newy = rng->getInt(1, height - 1);
		}

		dir = NONE; // reset the direction

		// Try to find a wall or corridor tile
		if(getTile(newx, newy).tileType == Tile::ROOM) continue; // Start over if the tile is a room, otherwise it's a wall or corridor
		
		if(getTile(newx, newy + 1).tileType == Tile::ROOM || getTile(newx, newy + 1).tileType == Tile::CORRIDOR) { // Check if the tile is the northern edge of a feature
			if(createFeature(newx, newy - 1, NORTH)) { // Try to create a feature to the north
				map->setProperties(newx, newy, true, true); // Clear a path to the new feature
				createDoor(newx, newy); // Add a door
				map->setProperties(newx, newy - 1, true, true); // Clean in front of door
				break;
			}
		}
		else if(getTile(newx - 1, newy).tileType == Tile::ROOM || getTile(newx - 1, newy).tileType == Tile::CORRIDOR) { // Check if the tile is on the eastern edge of a feature
			if(createFeature(newx + 1, newy, EAST)) { // Try to create a feature to the east
				map->setProperties(newx, newy, true, true); // Clear a path to the new feature
				createDoor(newx, newy); // Add a door
				map->setProperties(newx + 1, newy, true, true); // Clean in front of door
				break;
			}
		}
		else if(getTile(newx, newy - 1).tileType == Tile::ROOM || getTile(newx, newy - 1).tileType == Tile::CORRIDOR) { // Check if the tile is on the southern edge of a feature
			if(createFeature(newx, newy + 1, SOUTH)) { // Try to create a feature to the south
				map->setProperties(newx, newy, true, true); // Clear a path to the new feature
				createDoor(newx, newy); // Add a door
				map->setProperties(newx, newy + 1, true, true); // Clean in front of door
				break;
			}
		}
		else if(getTile(newx + 1, newy).tileType == Tile::ROOM || getTile(newx + 1, newy).tileType == Tile::CORRIDOR) { // Check if the tile is on the western edge of a feature
			if(createFeature(newx - 1, newy, WEST)) { // Try to create a feature to the west
				map->setProperties(newx, newy, true, true); // Clear a path to the new feature
				createDoor(newx, newy); // Add a door
				map->setProperties(newx - 1, newy, true, true); // Clean in front of door
				break;
			}
		}
	}
}

// Checks if a room/corridor/etc is able to be placed without going out of bounds or overlapping other features
bool Map::canPutFeature(const Rectangle & rect) const {
	if(rect.x < 1 || rect.y < 1 || rect.x + rect.width > width - 1
		|| rect.y + rect.height > height - 1) { // Check if rect is out of bounds
		return false;
	}
	for(int y = rect.y - 1; y < rect.y + rect.height + 1; ++y) { // Check an extra tile around the rectangle so that rooms dont accidentally connect
		for(int x = rect.x - 1; x < rect.x + rect.width + 1; ++x) {
			if(canWalk(x, y)) { return false; } // Check if this rect will overlap an existing rect
		}
	}
	return true;
}

bool Map::createFeature(int x, int y, Direction dir, Tile::TileType tileType) {
	const static int ROOM_CHANCE = 85;
	int chance = 0;

	// If tileType is explicitly defined set the chance to guarentee that TileType
	if(tileType == Tile::ROOM) 
		chance = 1;
	else if(tileType == Tile::CORRIDOR) 
		chance = ROOM_CHANCE + 1;
	else 
		chance = rng->getInt(1, 100);

	if(chance <= ROOM_CHANCE) {
		// Define a randomly sized rectangle
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
			rooms.push_back(room);
			++currentFeatures;
			return true; // Room successfully created
		}
	}
	else { // Corridor (for now)
		// Define a randomly long corridor
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
			break;
		}
		case SOUTH:
		{
			corridor.height = length;
			corridor.width = 1;
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
			++currentFeatures;
			if(currentFeatures < MAX_FEATURES) { generateFeature(&corridor); } // If a corridor is created, add a feature to it (a corridor would be made to go somewhere!)
			return true; // Corridor successfully created
		}
	}
	return false;
}


/*
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