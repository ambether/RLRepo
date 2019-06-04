#pragma once

class Map {
public:
	Map(int width, int height);
	~Map();

	int getWidth() const;
	int getHeight() const;

	bool canWalk(int x, int y) const;
	bool isWall(int x, int y) const;
	bool isExplored(int x, int y) const;
	bool isInFov(int x, int y) const;

	void computeFov();
	void render(TCODConsole * renderConsole) const;
	void setTransparent(int x, int y, bool transparent);

private:
	struct Tile {
		bool isExplored;
		enum TileType { NONE, WALL, ROOM, CORRIDOR } tileType;
		Tile() : isExplored(false), tileType(WALL) {}
	};
	struct Rectangle {
		int x, y;
		int width, height;
	};

	const static int ROOM_SIZE_MIN = 6, ROOM_SIZE_MAX = 15,
		MAX_FEATURES = 15;
	enum Direction { NONE, NORTH, EAST, SOUTH, WEST };

	int width, height;
	int currentFeatures;
	Tile * tiles;
	vector<Rectangle> rooms;
	shared_ptr<TCODMap> map;
	shared_ptr<TCODRandom> rng;

	Tile & getTile(int x, int y) const;

	void generateMap();
	void generateFeature(Rectangle * bounds = nullptr);
	void dig(const Rectangle & rect, Tile::TileType tileType);
	void createDoor(int x, int y);
	void placeEntities();
	void addMonster(int x, int y);
	void addItem(int x, int y);

	bool canPutFeature(const Rectangle & rect) const;
	bool createFeature(int x, int y, Direction dir, Tile::TileType tileType = Tile::NONE);
};