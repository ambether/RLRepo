#pragma once

struct Tile {
	//bool canWalk;
	bool isExplored;

	//Tile() : canWalk(false), isExplored(false) {}
	Tile() : isExplored(false) {}
};

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
	//void addMonster(int x, int y);
	//void addDoor(int x, int y);
	//void placeDoors();
	void setTransparent(int x, int y, bool transparent);

private:
	struct Rectangle {
		int x, y;
		int width, height;
	};

	const static int ROOM_SIZE_MIN = 6, ROOM_SIZE_MAX = 25,
		MAX_FEATURES = 15;
	
	const enum Direction { NONE, NORTH, EAST, SOUTH, WEST };

	int width, height;
	Tile * exploredTiles;
	shared_ptr<TCODMap> map;
	shared_ptr<TCODRandom> rng;

	void generateMap();
	void dig(const Rectangle & rect);

	bool canPutRoom(const Rectangle & room) const;
	bool createRoom(int x, int y, Direction dir);


	// TODO: full map gen

	//struct RoomData {
	//	int x, y, w, h;
	//	RoomData(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
	//};

	//vector<RoomData> rooms;

	//friend class bspList;
	
	//void createRoom(bool first, int x1, int y1, int x2, int y2);
	//void addItem(int x, int y);
};