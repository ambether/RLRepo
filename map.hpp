#pragma once

struct Tile {
	bool canWalk;
	bool isExplored;

	Tile() : canWalk(false), isExplored(false) {}
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
	void addMonster(int x, int y);
	void addDoor(int x, int y);
	void placeDoors();
	void setTransparent(int x, int y, bool transparent);

private:
	struct RoomData {
		int x, y, w, h;
		RoomData(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
	};

	int width, height;
	Tile * tiles;
	std::shared_ptr<TCODMap> map;

	std::vector<RoomData> rooms;

	friend class bspList;
	
	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2);
	void addItem(int x, int y);
};